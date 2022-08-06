cbuffer CoordModifierBlock : register(b0)
{
    float2 _26_uScreenCoordsModifier : packoffset(c0);
};


static float4 gl_Position;
static float2 fTextureCoords;
static float2 vTextureCoords;
static float2 vPosition;

struct SPIRV_Cross_Input
{
    float2 vPosition : TEXCOORD0;
    float2 vTextureCoords : TEXCOORD1;
};

struct SPIRV_Cross_Output
{
    float2 fTextureCoords : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    fTextureCoords = vTextureCoords;
    gl_Position = float4(vPosition * _26_uScreenCoordsModifier, 0.0f, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    vTextureCoords = stage_input.vTextureCoords;
    vPosition = stage_input.vPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.fTextureCoords = fTextureCoords;
    return stage_output;
}
