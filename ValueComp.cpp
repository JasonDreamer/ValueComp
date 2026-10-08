#include "ValueComp.h"

static PF_Err About(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef*[],
    PF_LayerDef*)
{
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    suites.ANSICallbacksSuite1()->sprintf(
        out_data->return_msg,
        "%s v%d.%d\r%s",
        STR(StrID_Name),
        MAJOR_VERSION,
        MINOR_VERSION,
        STR(StrID_Description));
    return PF_Err_NONE;
}

static PF_Err GlobalSetup(
    PF_InData*,
    PF_OutData* out_data,
    PF_ParamDef*[],
    PF_LayerDef*)
{
    out_data->my_version = PF_VERSION(
        MAJOR_VERSION,
        MINOR_VERSION,
        BUG_VERSION,
        STAGE_VERSION,
        BUILD_VERSION);

    out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE |
                          PF_OutFlag_PIX_INDEPENDENT;
    out_data->out_flags2 = PF_OutFlag2_FLOAT_COLOR_AWARE |
                           PF_OutFlag2_SUPPORTS_SMART_RENDER;
    return PF_Err_NONE;
}

static PF_Err ParamsSetup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef*[],
    PF_LayerDef*)
{
    PF_Err err = PF_Err_NONE;
    PF_ParamDef def;

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_Compression_Param_Name),
        VALUECOMP_COMPRESSION_MIN,
        VALUECOMP_COMPRESSION_MAX,
        VALUECOMP_COMPRESSION_MIN,
        VALUECOMP_COMPRESSION_MAX,
        VALUECOMP_COMPRESSION_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        COMPRESSION_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_GroupRange_Param_Name),
        VALUECOMP_GROUP_RANGE_MIN,
        VALUECOMP_GROUP_RANGE_MAX,
        VALUECOMP_GROUP_RANGE_MIN,
        VALUECOMP_GROUP_RANGE_MAX,
        VALUECOMP_GROUP_RANGE_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        GROUP_RANGE_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_Softness_Param_Name),
        VALUECOMP_SOFTNESS_MIN,
        VALUECOMP_SOFTNESS_MAX,
        VALUECOMP_SOFTNESS_MIN,
        VALUECOMP_SOFTNESS_MAX,
        VALUECOMP_SOFTNESS_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        SOFTNESS_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_ChromaInfluence_Param_Name),
        VALUECOMP_CHROMA_INFLUENCE_MIN,
        VALUECOMP_CHROMA_INFLUENCE_MAX,
        VALUECOMP_CHROMA_INFLUENCE_MIN,
        VALUECOMP_CHROMA_INFLUENCE_MAX,
        VALUECOMP_CHROMA_INFLUENCE_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        CHROMA_INFLUENCE_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_POPUP(
        STR(StrID_View_Param_Name),
        2,
        VALUECOMP_VIEW_RESULT,
        "Result|Value Map",
        VIEW_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_TOPIC(
        STR(StrID_HiddenColor_Group_Name),
        HIDDEN_COLOR_START_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_ColorAmount_Param_Name),
        VALUECOMP_COLOR_AMOUNT_MIN,
        VALUECOMP_COLOR_AMOUNT_MAX,
        VALUECOMP_COLOR_AMOUNT_MIN,
        VALUECOMP_COLOR_AMOUNT_MAX,
        VALUECOMP_COLOR_AMOUNT_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        COLOR_AMOUNT_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_ANGLE(
        STR(StrID_ShadowHue_Param_Name),
        static_cast<A_long>(VALUECOMP_SHADOW_HUE_DFLT),
        SHADOW_HUE_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_ANGLE(
        STR(StrID_HighlightHue_Param_Name),
        static_cast<A_long>(VALUECOMP_HIGHLIGHT_HUE_DFLT),
        HIGHLIGHT_HUE_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_ChromaLift_Param_Name),
        VALUECOMP_CHROMA_LIFT_MIN,
        VALUECOMP_CHROMA_LIFT_MAX,
        VALUECOMP_CHROMA_LIFT_MIN,
        VALUECOMP_CHROMA_LIFT_MAX,
        VALUECOMP_CHROMA_LIFT_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        CHROMA_LIFT_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_ColorBalance_Param_Name),
        VALUECOMP_COLOR_BALANCE_MIN,
        VALUECOMP_COLOR_BALANCE_MAX,
        VALUECOMP_COLOR_BALANCE_MIN,
        VALUECOMP_COLOR_BALANCE_MAX,
        VALUECOMP_COLOR_BALANCE_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        COLOR_BALANCE_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_ColorNotes_Param_Name),
        VALUECOMP_COLOR_NOTES_MIN,
        VALUECOMP_COLOR_NOTES_MAX,
        VALUECOMP_COLOR_NOTES_MIN,
        VALUECOMP_COLOR_NOTES_MAX,
        VALUECOMP_COLOR_NOTES_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        COLOR_NOTES_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_NotesScale_Param_Name),
        VALUECOMP_NOTES_SCALE_MIN,
        VALUECOMP_NOTES_SCALE_MAX,
        VALUECOMP_NOTES_SCALE_MIN,
        VALUECOMP_NOTES_SCALE_MAX,
        VALUECOMP_NOTES_SCALE_DFLT,
        PF_Precision_TENTHS,
        0,
        0,
        NOTES_SCALE_DISK_ID);

    AEFX_CLR_STRUCT(def);
    PF_END_TOPIC(HIDDEN_COLOR_END_DISK_ID);

    out_data->num_params = VALUECOMP_NUM_PARAMS;
    return err;
}

static ValueCompParams BuildParams(
    const PF_ParamDef& compression,
    const PF_ParamDef& groupRange,
    const PF_ParamDef& softness,
    const PF_ParamDef& chromaInfluence,
    const PF_ParamDef& view,
    const PF_ParamDef& colorAmount,
    const PF_ParamDef& shadowHue,
    const PF_ParamDef& highlightHue,
    const PF_ParamDef& chromaLift,
    const PF_ParamDef& colorBalance,
    const PF_ParamDef& colorNotes,
    const PF_ParamDef& notesScale)
{
    ValueCompParams result;
    result.compression = static_cast<float>(compression.u.fs_d.value / 100.0);
    result.groupRange = static_cast<float>(groupRange.u.fs_d.value / 100.0);
    result.softness = static_cast<float>(softness.u.fs_d.value / 100.0);
    result.chromaInfluence = static_cast<float>(chromaInfluence.u.fs_d.value / 100.0);
    result.viewMode = view.u.pd.value;
    result.colorAmount = static_cast<float>(colorAmount.u.fs_d.value / 100.0);
    result.shadowHueDegrees = static_cast<float>(shadowHue.u.ad.value / 65536.0);
    result.highlightHueDegrees = static_cast<float>(highlightHue.u.ad.value / 65536.0);
    result.chromaLift = static_cast<float>(chromaLift.u.fs_d.value / 100.0);
    result.colorBalance = static_cast<float>(colorBalance.u.fs_d.value / 100.0);
    result.colorNotes = static_cast<float>(colorNotes.u.fs_d.value / 100.0);
    result.notesScale = static_cast<float>(notesScale.u.fs_d.value);
    result.originX = 0;
    result.originY = 0;
    return result;
}

static PF_Err GetBitDepth(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_EffectWorld* world,
    int* bitDepth)
{
    PF_Err err = PF_Err_NONE;
    PF_WorldSuite2* worldSuite = NULL;
    PF_PixelFormat format = PF_PixelFormat_INVALID;
    const void* acquiredSuite = NULL;

    const SPErr acquireErr = in_data->pica_basicP->AcquireSuite(
        kPFWorldSuite,
        kPFWorldSuiteVersion2,
        &acquiredSuite);

    if (acquireErr != 0 || !acquiredSuite) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    worldSuite = const_cast<PF_WorldSuite2*>(
        static_cast<const PF_WorldSuite2*>(acquiredSuite));
    err = worldSuite->PF_GetPixelFormat(world, &format);

    if (!err) {
        switch (format) {
            case PF_PixelFormat_ARGB128:
                *bitDepth = 32;
                break;
            case PF_PixelFormat_ARGB64:
                *bitDepth = 16;
                break;
            case PF_PixelFormat_ARGB32:
                *bitDepth = 8;
                break;
            default:
                err = PF_Err_BAD_CALLBACK_PARAM;
                break;
        }
    }

    const SPErr releaseErr = in_data->pica_basicP->ReleaseSuite(
        kPFWorldSuite,
        kPFWorldSuiteVersion2);
    if (!err && releaseErr != 0) {
        err = PF_Err_BAD_CALLBACK_PARAM;
    }
    return err;
}

static PF_Err CopyWorld(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    ERR(PF_COPY(input, output, NULL, NULL));
    return err;
}

static PF_Err ProcessWorlds(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const ValueCompParams& params)
{
    if (!input || !output || !input->data || !output->data) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    if (params.compression <= 0.0f &&
        params.colorAmount <= 0.0f &&
        params.colorNotes <= 0.0f &&
        params.viewMode == VALUECOMP_VIEW_RESULT) {
        return CopyWorld(in_data, input, output);
    }

    int bitDepth = 8;
    PF_Err err = GetBitDepth(in_data, out_data, output, &bitDepth);
    if (err) {
        return CopyWorld(in_data, input, output);
    }

    ValueCompParams renderParams = params;
    renderParams.originX = in_data->output_origin_x;
    renderParams.originY = in_data->output_origin_y;
    if (in_data->downsample_x.den > 0) {
        renderParams.notesScale *= static_cast<float>(in_data->downsample_x.num) /
                                   static_cast<float>(in_data->downsample_x.den);
    }
    renderParams.notesScale = renderParams.notesScale < 1.0f ? 1.0f : renderParams.notesScale;

    const bool cudaSuccess = LaunchValueCompKernel(
        input->data,
        output->data,
        output->width,
        output->height,
        input->rowbytes,
        output->rowbytes,
        bitDepth,
        renderParams);

    return cudaSuccess ? PF_Err_NONE : CopyWorld(in_data, input, output);
}

static PF_Err Render(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    const ValueCompParams effectParams = BuildParams(
        *params[VALUECOMP_COMPRESSION],
        *params[VALUECOMP_GROUP_RANGE],
        *params[VALUECOMP_SOFTNESS],
        *params[VALUECOMP_CHROMA_INFLUENCE],
        *params[VALUECOMP_VIEW],
        *params[VALUECOMP_COLOR_AMOUNT],
        *params[VALUECOMP_SHADOW_HUE],
        *params[VALUECOMP_HIGHLIGHT_HUE],
        *params[VALUECOMP_CHROMA_LIFT],
        *params[VALUECOMP_COLOR_BALANCE],
        *params[VALUECOMP_COLOR_NOTES],
        *params[VALUECOMP_NOTES_SCALE]);

    return ProcessWorlds(
        in_data,
        out_data,
        &params[VALUECOMP_INPUT]->u.ld,
        output,
        effectParams);
}

static PF_Err PreRender(
    PF_InData* in_data,
    PF_OutData*,
    PF_PreRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_RenderRequest request = extra->input->output_request;
    PF_CheckoutResult inputResult;
    request.preserve_rgb_of_zero_alpha = TRUE;

    ERR(extra->cb->checkout_layer(
        in_data->effect_ref,
        VALUECOMP_INPUT,
        VALUECOMP_INPUT,
        &request,
        in_data->current_time,
        in_data->time_step,
        in_data->time_scale,
        &inputResult));

    if (!err) {
        extra->output->result_rect = inputResult.result_rect;
        extra->output->max_result_rect = inputResult.max_result_rect;
    }
    return err;
}

static PF_Err SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_EffectWorld* input = NULL;
    PF_EffectWorld* output = NULL;
    PF_ParamDef compression;
    PF_ParamDef groupRange;
    PF_ParamDef softness;
    PF_ParamDef chromaInfluence;
    PF_ParamDef view;
    PF_ParamDef colorAmount;
    PF_ParamDef shadowHue;
    PF_ParamDef highlightHue;
    PF_ParamDef chromaLift;
    PF_ParamDef colorBalance;
    PF_ParamDef colorNotes;
    PF_ParamDef notesScale;
    bool compressionChecked = false;
    bool groupRangeChecked = false;
    bool softnessChecked = false;
    bool chromaInfluenceChecked = false;
    bool viewChecked = false;
    bool colorAmountChecked = false;
    bool shadowHueChecked = false;
    bool highlightHueChecked = false;
    bool chromaLiftChecked = false;
    bool colorBalanceChecked = false;
    bool colorNotesChecked = false;
    bool notesScaleChecked = false;

    AEFX_CLR_STRUCT(compression);
    AEFX_CLR_STRUCT(groupRange);
    AEFX_CLR_STRUCT(softness);
    AEFX_CLR_STRUCT(chromaInfluence);
    AEFX_CLR_STRUCT(view);
    AEFX_CLR_STRUCT(colorAmount);
    AEFX_CLR_STRUCT(shadowHue);
    AEFX_CLR_STRUCT(highlightHue);
    AEFX_CLR_STRUCT(chromaLift);
    AEFX_CLR_STRUCT(colorBalance);
    AEFX_CLR_STRUCT(colorNotes);
    AEFX_CLR_STRUCT(notesScale);

    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, VALUECOMP_INPUT, &input));
    if (!err) {
        ERR(extra->cb->checkout_output(in_data->effect_ref, &output));
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_COMPRESSION, in_data->current_time,
            in_data->time_step, in_data->time_scale, &compression));
        compressionChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_GROUP_RANGE, in_data->current_time,
            in_data->time_step, in_data->time_scale, &groupRange));
        groupRangeChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_SOFTNESS, in_data->current_time,
            in_data->time_step, in_data->time_scale, &softness));
        softnessChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_CHROMA_INFLUENCE, in_data->current_time,
            in_data->time_step, in_data->time_scale, &chromaInfluence));
        chromaInfluenceChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_VIEW, in_data->current_time,
            in_data->time_step, in_data->time_scale, &view));
        viewChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_COLOR_AMOUNT, in_data->current_time,
            in_data->time_step, in_data->time_scale, &colorAmount));
        colorAmountChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_SHADOW_HUE, in_data->current_time,
            in_data->time_step, in_data->time_scale, &shadowHue));
        shadowHueChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_HIGHLIGHT_HUE, in_data->current_time,
            in_data->time_step, in_data->time_scale, &highlightHue));
        highlightHueChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_CHROMA_LIFT, in_data->current_time,
            in_data->time_step, in_data->time_scale, &chromaLift));
        chromaLiftChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_COLOR_BALANCE, in_data->current_time,
            in_data->time_step, in_data->time_scale, &colorBalance));
        colorBalanceChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_COLOR_NOTES, in_data->current_time,
            in_data->time_step, in_data->time_scale, &colorNotes));
        colorNotesChecked = !err;
    }
    if (!err) {
        ERR(PF_CHECKOUT_PARAM(in_data, VALUECOMP_NOTES_SCALE, in_data->current_time,
            in_data->time_step, in_data->time_scale, &notesScale));
        notesScaleChecked = !err;
    }

    if (!err) {
        const ValueCompParams effectParams = BuildParams(
            compression,
            groupRange,
            softness,
            chromaInfluence,
            view,
            colorAmount,
            shadowHue,
            highlightHue,
            chromaLift,
            colorBalance,
            colorNotes,
            notesScale);
        err = ProcessWorlds(in_data, out_data, input, output, effectParams);
    } else if (input && output) {
        err = CopyWorld(in_data, input, output);
    }

    if (notesScaleChecked) {
        PF_CHECKIN_PARAM(in_data, &notesScale);
    }
    if (colorNotesChecked) {
        PF_CHECKIN_PARAM(in_data, &colorNotes);
    }
    if (colorBalanceChecked) {
        PF_CHECKIN_PARAM(in_data, &colorBalance);
    }
    if (chromaLiftChecked) {
        PF_CHECKIN_PARAM(in_data, &chromaLift);
    }
    if (highlightHueChecked) {
        PF_CHECKIN_PARAM(in_data, &highlightHue);
    }
    if (shadowHueChecked) {
        PF_CHECKIN_PARAM(in_data, &shadowHue);
    }
    if (colorAmountChecked) {
        PF_CHECKIN_PARAM(in_data, &colorAmount);
    }
    if (viewChecked) {
        PF_CHECKIN_PARAM(in_data, &view);
    }
    if (chromaInfluenceChecked) {
        PF_CHECKIN_PARAM(in_data, &chromaInfluence);
    }
    if (softnessChecked) {
        PF_CHECKIN_PARAM(in_data, &softness);
    }
    if (groupRangeChecked) {
        PF_CHECKIN_PARAM(in_data, &groupRange);
    }
    if (compressionChecked) {
        PF_CHECKIN_PARAM(in_data, &compression);
    }
    if (input) {
        PF_Err checkinErr = extra->cb->checkin_layer_pixels(in_data->effect_ref, VALUECOMP_INPUT);
        if (!err) {
            err = checkinErr;
        }
    }
    return err;
}

extern "C" DllExport
PF_Err PluginDataEntryFunction2(
    PF_PluginDataPtr inPtr,
    PF_PluginDataCB2 inPluginDataCallBackPtr,
    SPBasicSuite*,
    const char*,
    const char*)
{
    PF_Err result = PF_Err_INVALID_CALLBACK;

    result = PF_REGISTER_EFFECT_EXT2(
        inPtr,
        inPluginDataCallBackPtr,
        "ValueComp",
        "mknb:ValueComp",
        "mknb_Plugin",
        AE_RESERVED_INFO,
        "EffectMain",
        "https://github.com/mknb");

    return result;
}

PF_Err EffectMain(
    PF_Cmd cmd,
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    void* extra)
{
    PF_Err err = PF_Err_NONE;
    try {
        switch (cmd) {
            case PF_Cmd_ABOUT:
                err = About(in_data, out_data, params, output);
                break;
            case PF_Cmd_GLOBAL_SETUP:
                err = GlobalSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_PARAMS_SETUP:
                err = ParamsSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_RENDER:
                err = Render(in_data, out_data, params, output);
                break;
            case PF_Cmd_SMART_PRE_RENDER:
                err = PreRender(in_data, out_data, reinterpret_cast<PF_PreRenderExtra*>(extra));
                break;
            case PF_Cmd_SMART_RENDER:
                err = SmartRender(in_data, out_data, reinterpret_cast<PF_SmartRenderExtra*>(extra));
                break;
        }
    }
    catch (PF_Err& thrownErr) {
        err = thrownErr;
    }
    return err;
}
