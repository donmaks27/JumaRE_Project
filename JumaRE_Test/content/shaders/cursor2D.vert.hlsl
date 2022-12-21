cbuffer ModifierBlock : register(b0)
{
    float2 _19_uLocation : packoffset(c0);
    float2 _19_uSize : packoffset(c0.z);
    float2 _19_uOffset : packoffset(c1);
    float _19_uDepth : packoffset(c1.z);
};


static float4 gl_Position;
static float2 vPosition;
static float2 vTexCoords;

struct SPIRV_Cross_Input
{
    float2 vPosition : TEXCOORD0;
    float2 vTexCoords : TEXCOORD1;
};

struct SPIRV_Cross_Output
{
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    gl_Position = float4(_19_uLocation + (_19_uSize * (_19_uOffset + vPosition)), _19_uDepth, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    vPosition = stage_input.vPosition;
    vTexCoords = stage_input.vTexCoords;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    return stage_output;
}
