#pragma once

#ifndef VALUECOMP_CUDA_CUH
#define VALUECOMP_CUDA_CUH

typedef struct ValueCompParams {
    float compression;
    float groupRange;
    float softness;
    float chromaInfluence;
    int viewMode;
    float colorAmount;
    float shadowHueDegrees;
    float highlightHueDegrees;
    float chromaLift;
    float colorBalance;
    float colorNotes;
    float notesScale;
    int originX;
    int originY;
} ValueCompParams;

#define VALUECOMP_VIEW_RESULT 1
#define VALUECOMP_VIEW_VALUE_MAP 2

#ifdef __cplusplus
extern "C" {
#endif

bool LaunchValueCompKernel(
    const void* input,
    void* output,
    int width,
    int height,
    int inputRowBytes,
    int outputRowBytes,
    int bitDepth,
    ValueCompParams params);

#ifdef __cplusplus
}
#endif

#endif
