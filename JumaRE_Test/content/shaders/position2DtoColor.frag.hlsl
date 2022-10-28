static float4 outColor;
static float2 fPosition;

struct SPIRV_Cross_Input
{
    float2 fPosition : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 outColor : SV_Target0;
};

void frag_main()
{
    outColor = float4(fPosition.x, fPosition.y, 0.0f, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    fPosition = stage_input.fPosition;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.outColor = outColor;
    return stage_output;
}
