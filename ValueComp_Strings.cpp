#include "ValueComp.h"

typedef struct {
    A_u_long index;
    A_char str[256];
} TableString;

TableString g_strs[StrID_NUMTYPES] = {
    { StrID_NONE, "" },
    { StrID_Name, "ValueComp" },
    { StrID_Description, "Compresses perceptual values and enriches shadows and highlights with value-preserving hidden color." },
    { StrID_Compression_Param_Name, "Compression" },
    { StrID_GroupRange_Param_Name, "Group Range" },
    { StrID_Softness_Param_Name, "Band Softness" },
    { StrID_ChromaInfluence_Param_Name, "Chroma Influence" },
    { StrID_View_Param_Name, "View" },
    { StrID_HiddenColor_Group_Name, "Hidden Color" },
    { StrID_ColorAmount_Param_Name, "Amount" },
    { StrID_ShadowHue_Param_Name, "Shadow Hue" },
    { StrID_HighlightHue_Param_Name, "Highlight Hue" },
    { StrID_ChromaLift_Param_Name, "Chroma Lift" },
    { StrID_ColorBalance_Param_Name, "Balance" },
    { StrID_ColorNotes_Param_Name, "Color Notes" },
    { StrID_NotesScale_Param_Name, "Notes Scale" }
};

char* GetStringPtr(int strNum)
{
    return g_strs[strNum].str;
}
