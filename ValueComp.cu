#include <cuda_runtime.h>
#include <math.h>

#include "ValueComp_CUDA.cuh"

struct Pixel8 {
    unsigned char alpha;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct Pixel16 {
    unsigned short alpha;
    unsigned short red;
    unsigned short green;
    unsigned short blue;
};

struct Pixel32 {
    float alpha;
    float red;
    float green;
    float blue;
};

struct Float3 {
    float x;
    float y;
    float z;
};

__device__ __forceinline__ float clampValue(float value, float low, float high)
{
    return fminf(fmaxf(value, low), high);
}

__device__ __forceinline__ float srgbToLinear(float value)
{
    const float sign = value < 0.0f ? -1.0f : 1.0f;
    const float magnitude = fabsf(value);
    const float linear = magnitude <= 0.04045f
        ? magnitude / 12.92f
        : powf((magnitude + 0.055f) / 1.055f, 2.4f);
    return sign * linear;
}

__device__ __forceinline__ float linearToSrgb(float value)
{
    const float sign = value < 0.0f ? -1.0f : 1.0f;
    const float magnitude = fabsf(value);
    const float encoded = magnitude <= 0.0031308f
        ? magnitude * 12.92f
        : 1.055f * powf(magnitude, 1.0f / 2.4f) - 0.055f;
    return sign * encoded;
}

__device__ __forceinline__ Float3 linearRgbToOklab(float r, float g, float b)
{
    const float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
    const float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
    const float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    const float lc = cbrtf(l);
    const float mc = cbrtf(m);
    const float sc = cbrtf(s);

    Float3 lab;
    lab.x = 0.2104542553f * lc + 0.7936177850f * mc - 0.0040720468f * sc;
    lab.y = 1.9779984951f * lc - 2.4285922050f * mc + 0.4505937099f * sc;
    lab.z = 0.0259040371f * lc + 0.7827717662f * mc - 0.8086757660f * sc;
    return lab;
}

__device__ __forceinline__ Float3 oklabToLinearRgb(float lightness, float a, float b)
{
    const float lc = lightness + 0.3963377774f * a + 0.2158037573f * b;
    const float mc = lightness - 0.1055613458f * a - 0.0638541728f * b;
    const float sc = lightness - 0.0894841775f * a - 1.2914855480f * b;
    const float l = lc * lc * lc;
    const float m = mc * mc * mc;
    const float s = sc * sc * sc;

    Float3 rgb;
    rgb.x = 4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    rgb.y = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    rgb.z = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
    return rgb;
}

__device__ __forceinline__ float bandAttenuation(float distance, float groupRange, float softness)
{
    const float halfRange = 0.5f * groupRange;
    const float absoluteDistance = fabsf(distance);
    if (softness <= 0.0001f || absoluteDistance <= halfRange * (1.0f - softness)) {
        return 1.0f;
    }

    const float transitionStart = halfRange * (1.0f - softness);
    const float transitionWidth = fmaxf(halfRange - transitionStart, 0.000001f);
    const float t = clampValue((absoluteDistance - transitionStart) / transitionWidth, 0.0f, 1.0f);
    const float smooth = t * t * (3.0f - 2.0f * t);
    return 1.0f - smooth;
}

__device__ __forceinline__ float smoothstepValue(float edge0, float edge1, float value)
{
    const float width = fmaxf(edge1 - edge0, 0.000001f);
    const float t = clampValue((value - edge0) / width, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

__device__ __forceinline__ float hashNoise(int x, int y)
{
    unsigned int value = static_cast<unsigned int>(x) * 374761393u +
                         static_cast<unsigned int>(y) * 668265263u + 2246822519u;
    value = (value ^ (value >> 13)) * 1274126177u;
    value ^= value >> 16;
    return static_cast<float>(value & 0x00ffffffu) / 16777215.0f;
}

__device__ __forceinline__ float valueNoise(float x, float y)
{
    const int x0 = static_cast<int>(floorf(x));
    const int y0 = static_cast<int>(floorf(y));
    const float fx = x - floorf(x);
    const float fy = y - floorf(y);
    const float sx = fx * fx * (3.0f - 2.0f * fx);
    const float sy = fy * fy * (3.0f - 2.0f * fy);
    const float n00 = hashNoise(x0, y0);
    const float n10 = hashNoise(x0 + 1, y0);
    const float n01 = hashNoise(x0, y0 + 1);
    const float n11 = hashNoise(x0 + 1, y0 + 1);
    const float nx0 = n00 + (n10 - n00) * sx;
    const float nx1 = n01 + (n11 - n01) * sx;
    return nx0 + (nx1 - nx0) * sy;
}

__device__ __forceinline__ float compressValue(
    float value,
    const ValueCompParams& params)
{
    const float groupRange = fmaxf(params.groupRange, 0.001f);
    const float bandCenter = roundf((value - 0.5f) / groupRange) * groupRange + 0.5f;
    const float distance = value - bandCenter;
    const float attenuation = bandAttenuation(distance, groupRange, params.softness);
    return value - distance * params.compression * attenuation;
}

__device__ __forceinline__ bool isInGamut(const Float3& rgb, bool allowHdr)
{
    const bool aboveFloor = rgb.x >= 0.0f && rgb.y >= 0.0f && rgb.z >= 0.0f;
    if (!aboveFloor) {
        return false;
    }
    return allowHdr || (rgb.x <= 1.0f && rgb.y <= 1.0f && rgb.z <= 1.0f);
}

__device__ __forceinline__ Float3 mapChromaToGamut(
    float lightness,
    float a,
    float b,
    bool allowHdr)
{
    Float3 rgb = oklabToLinearRgb(lightness, a, b);
    if (isInGamut(rgb, allowHdr)) {
        return rgb;
    }

    float low = 0.0f;
    float high = 1.0f;
    for (int i = 0; i < 12; ++i) {
        const float scale = 0.5f * (low + high);
        const Float3 candidate = oklabToLinearRgb(lightness, a * scale, b * scale);
        if (isInGamut(candidate, allowHdr)) {
            low = scale;
            rgb = candidate;
        } else {
            high = scale;
        }
    }
    return rgb;
}

__device__ __forceinline__ Float3 processColor(
    float r,
    float g,
    float b,
    int x,
    int y,
    const ValueCompParams& params,
    bool allowHdr)
{
    const Float3 lab = linearRgbToOklab(
        srgbToLinear(r),
        srgbToLinear(g),
        srgbToLinear(b));
    const float chroma = sqrtf(lab.y * lab.y + lab.z * lab.z);
    const float effectiveValue = lab.x + params.chromaInfluence * 0.25f * chroma;
    const float compressedValue = compressValue(effectiveValue, params);

    if (params.viewMode == VALUECOMP_VIEW_VALUE_MAP) {
        const float previewLightness = allowHdr
            ? fmaxf(compressedValue, 0.0f)
            : clampValue(compressedValue, 0.0f, 1.0f);
        const float grayLinear = previewLightness * previewLightness * previewLightness;
        const float gray = linearToSrgb(grayLinear);
        Float3 preview = { gray, gray, gray };
        return preview;
    }

    float newA = lab.y;
    float newB = lab.z;
    const float tonalValue = clampValue(compressedValue, 0.0f, 1.0f);
    const float blackProtection = smoothstepValue(0.015f, 0.10f, tonalValue);
    const float whiteProtection = 1.0f - smoothstepValue(0.90f, 0.985f, tonalValue);
    const float colorMask = blackProtection * whiteProtection;

    if (params.colorAmount > 0.0f && colorMask > 0.0f) {
        const float balance = clampValue(params.colorBalance, 0.05f, 0.95f);
        const float highlightWeight = smoothstepValue(
            balance - 0.22f,
            balance + 0.22f,
            tonalValue);
        const float degreesToRadians = 0.01745329251994329577f;
        const float shadowHue = params.shadowHueDegrees * degreesToRadians;
        const float highlightHue = params.highlightHueDegrees * degreesToRadians;
        const float hueDelta = atan2f(
            sinf(highlightHue - shadowHue),
            cosf(highlightHue - shadowHue));
        const float targetHue = shadowHue + hueDelta * highlightWeight;
        const float targetA = cosf(targetHue);
        const float targetB = sinf(targetHue);

        const float mixAmount = clampValue(params.colorAmount * colorMask, 0.0f, 1.0f);
        const float desiredChroma = chroma + params.chromaLift * 0.08f * mixAmount;
        float mixedA = newA + (targetA * desiredChroma - newA) * mixAmount;
        float mixedB = newB + (targetB * desiredChroma - newB) * mixAmount;
        const float mixedLength = sqrtf(mixedA * mixedA + mixedB * mixedB);
        if (mixedLength > 0.000001f) {
            mixedA *= desiredChroma / mixedLength;
            mixedB *= desiredChroma / mixedLength;
        } else {
            mixedA = targetA * desiredChroma;
            mixedB = targetB * desiredChroma;
        }
        newA = mixedA;
        newB = mixedB;
    }

    if (params.colorNotes > 0.0f && colorMask > 0.0f) {
        const float scale = fmaxf(params.notesScale, 1.0f);
        const float note = valueNoise(
            static_cast<float>(x + params.originX) / scale,
            static_cast<float>(y + params.originY) / scale) * 2.0f - 1.0f;
        const float noteAngle = note * params.colorNotes * colorMask * 0.7853981633974483f;
        const float cosine = cosf(noteAngle);
        const float sine = sinf(noteAngle);
        const float rotatedA = newA * cosine - newB * sine;
        const float rotatedB = newA * sine + newB * cosine;
        newA = rotatedA;
        newB = rotatedB;
    }

    const float newChroma = sqrtf(newA * newA + newB * newB);
    float newLightness = compressedValue -
                         params.chromaInfluence * 0.25f * newChroma;
    newLightness = allowHdr
        ? fmaxf(newLightness, 0.0f)
        : clampValue(newLightness, 0.0f, 1.0f);

    const Float3 linear = mapChromaToGamut(newLightness, newA, newB, allowHdr);
    Float3 result = {
        linearToSrgb(linear.x),
        linearToSrgb(linear.y),
        linearToSrgb(linear.z)
    };
    if (!allowHdr) {
        result.x = clampValue(result.x, 0.0f, 1.0f);
        result.y = clampValue(result.y, 0.0f, 1.0f);
        result.z = clampValue(result.z, 0.0f, 1.0f);
    }
    return result;
}

__global__ void ValueCompKernel8(
    const void* input,
    void* output,
    int width,
    int height,
    int inputRowBytes,
    int outputRowBytes,
    ValueCompParams params)
{
    const int x = blockIdx.x * blockDim.x + threadIdx.x;
    const int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) {
        return;
    }

    const Pixel8* inputRow = reinterpret_cast<const Pixel8*>(
        reinterpret_cast<const char*>(input) + static_cast<size_t>(y) * inputRowBytes);
    Pixel8* outputRow = reinterpret_cast<Pixel8*>(
        reinterpret_cast<char*>(output) + static_cast<size_t>(y) * outputRowBytes);
    const Pixel8 source = inputRow[x];
    if (source.alpha == 0) {
        outputRow[x] = source;
        return;
    }
    const float alpha = source.alpha / 255.0f;
    const float unpremultiply = alpha > 0.000001f ? 1.0f / alpha : 1.0f;
    const Float3 color = processColor(
        source.red / 255.0f * unpremultiply,
        source.green / 255.0f * unpremultiply,
        source.blue / 255.0f * unpremultiply,
        x,
        y,
        params,
        false);

    outputRow[x].alpha = source.alpha;
    outputRow[x].red = static_cast<unsigned char>(clampValue(color.x * alpha, 0.0f, 1.0f) * 255.0f + 0.5f);
    outputRow[x].green = static_cast<unsigned char>(clampValue(color.y * alpha, 0.0f, 1.0f) * 255.0f + 0.5f);
    outputRow[x].blue = static_cast<unsigned char>(clampValue(color.z * alpha, 0.0f, 1.0f) * 255.0f + 0.5f);
}

__global__ void ValueCompKernel16(
    const void* input,
    void* output,
    int width,
    int height,
    int inputRowBytes,
    int outputRowBytes,
    ValueCompParams params)
{
    const int x = blockIdx.x * blockDim.x + threadIdx.x;
    const int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) {
        return;
    }

    const Pixel16* inputRow = reinterpret_cast<const Pixel16*>(
        reinterpret_cast<const char*>(input) + static_cast<size_t>(y) * inputRowBytes);
    Pixel16* outputRow = reinterpret_cast<Pixel16*>(
        reinterpret_cast<char*>(output) + static_cast<size_t>(y) * outputRowBytes);
    const Pixel16 source = inputRow[x];
    if (source.alpha == 0) {
        outputRow[x] = source;
        return;
    }
    const float maxChannel = 32768.0f;
    const float alpha = source.alpha / maxChannel;
    const float unpremultiply = alpha > 0.000001f ? 1.0f / alpha : 1.0f;
    const Float3 color = processColor(
        source.red / maxChannel * unpremultiply,
        source.green / maxChannel * unpremultiply,
        source.blue / maxChannel * unpremultiply,
        x,
        y,
        params,
        false);

    outputRow[x].alpha = source.alpha;
    outputRow[x].red = static_cast<unsigned short>(clampValue(color.x * alpha, 0.0f, 1.0f) * maxChannel + 0.5f);
    outputRow[x].green = static_cast<unsigned short>(clampValue(color.y * alpha, 0.0f, 1.0f) * maxChannel + 0.5f);
    outputRow[x].blue = static_cast<unsigned short>(clampValue(color.z * alpha, 0.0f, 1.0f) * maxChannel + 0.5f);
}

__global__ void ValueCompKernel32(
    const void* input,
    void* output,
    int width,
    int height,
    int inputRowBytes,
    int outputRowBytes,
    ValueCompParams params)
{
    const int x = blockIdx.x * blockDim.x + threadIdx.x;
    const int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) {
        return;
    }

    const Pixel32* inputRow = reinterpret_cast<const Pixel32*>(
        reinterpret_cast<const char*>(input) + static_cast<size_t>(y) * inputRowBytes);
    Pixel32* outputRow = reinterpret_cast<Pixel32*>(
        reinterpret_cast<char*>(output) + static_cast<size_t>(y) * outputRowBytes);
    const Pixel32 source = inputRow[x];
    if (!isfinite(source.alpha) || !isfinite(source.red) ||
        !isfinite(source.green) || !isfinite(source.blue) ||
        source.alpha <= 0.000001f) {
        outputRow[x] = source;
        return;
    }
    const float alpha = source.alpha;
    const float unpremultiply = alpha > 0.000001f ? 1.0f / alpha : 1.0f;
    const Float3 color = processColor(
        source.red * unpremultiply,
        source.green * unpremultiply,
        source.blue * unpremultiply,
        x,
        y,
        params,
        true);

    outputRow[x].alpha = source.alpha;
    outputRow[x].red = color.x * alpha;
    outputRow[x].green = color.y * alpha;
    outputRow[x].blue = color.z * alpha;
}

extern "C" bool LaunchValueCompKernel(
    const void* input,
    void* output,
    int width,
    int height,
    int inputRowBytes,
    int outputRowBytes,
    int bitDepth,
    ValueCompParams params)
{
    if (!input || !output || width <= 0 || height <= 0 ||
        inputRowBytes <= 0 || outputRowBytes <= 0) {
        return false;
    }

    const size_t inputSize = static_cast<size_t>(inputRowBytes) * static_cast<size_t>(height);
    const size_t outputSize = static_cast<size_t>(outputRowBytes) * static_cast<size_t>(height);
    void* deviceInput = NULL;
    void* deviceOutput = NULL;
    bool success = false;

    cudaError_t cudaError = cudaMalloc(&deviceInput, inputSize);
    if (cudaError != cudaSuccess) {
        goto cleanup;
    }
    cudaError = cudaMalloc(&deviceOutput, outputSize);
    if (cudaError != cudaSuccess) {
        goto cleanup;
    }
    cudaError = cudaMemcpy(deviceInput, input, inputSize, cudaMemcpyHostToDevice);
    if (cudaError != cudaSuccess) {
        goto cleanup;
    }
    cudaError = cudaMemset(deviceOutput, 0, outputSize);
    if (cudaError != cudaSuccess) {
        goto cleanup;
    }

    {
        const dim3 blockSize(16, 16);
        const dim3 gridSize(
            (width + blockSize.x - 1) / blockSize.x,
            (height + blockSize.y - 1) / blockSize.y);
        if (bitDepth == 32) {
            ValueCompKernel32<<<gridSize, blockSize>>>(
                deviceInput, deviceOutput, width, height,
                inputRowBytes, outputRowBytes, params);
        } else if (bitDepth == 16) {
            ValueCompKernel16<<<gridSize, blockSize>>>(
                deviceInput, deviceOutput, width, height,
                inputRowBytes, outputRowBytes, params);
        } else if (bitDepth == 8) {
            ValueCompKernel8<<<gridSize, blockSize>>>(
                deviceInput, deviceOutput, width, height,
                inputRowBytes, outputRowBytes, params);
        } else {
            goto cleanup;
        }
    }

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess) {
        goto cleanup;
    }
    cudaError = cudaDeviceSynchronize();
    if (cudaError != cudaSuccess) {
        goto cleanup;
    }
    cudaError = cudaMemcpy(output, deviceOutput, outputSize, cudaMemcpyDeviceToHost);
    if (cudaError != cudaSuccess) {
        goto cleanup;
    }
    success = true;

cleanup:
    if (deviceOutput) {
        cudaFree(deviceOutput);
    }
    if (deviceInput) {
        cudaFree(deviceInput);
    }
    return success;
}
