Texture2D<float4> uTexture : register(t0);
SamplerState _uTexture_sampler : register(s0);

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
    outColor = uTexture.Sample(_uTexture_sampler, fTextureCoords);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    fTextureCoords = stage_input.fTextureCoords;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.outColor = outColor;
    return stage_output;
}
