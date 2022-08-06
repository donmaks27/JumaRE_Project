static float4 outColor;
static float2 fTextureCoords;

struct SPIRV_Cross_Input
{
    float2 fTextureCoords : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 outColor : SV_Target0;
};

void frag_main()
{
    outColor = float4(fTextureCoords.x, fTextureCoords.y, 0.0f, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    fTextureCoords = stage_input.fTextureCoords;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.outColor = outColor;
    return stage_output;
}
