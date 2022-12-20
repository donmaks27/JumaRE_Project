cbuffer ModifierBlock : register(b0)
{
    row_major float4x4 _19_uModelMatrix : packoffset(c0);
    row_major float4x4 _19_uViewMatrix : packoffset(c4);
    row_major float4x4 _19_uProjectionMatrix : packoffset(c8);
};


static float4 gl_Position;
static float2 fTextureCoords;
static float2 vTextureCoords;
static float3 vPosition;

struct SPIRV_Cross_Input
{
    float3 vPosition : TEXCOORD0;
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
    float4 position = mul(float4(vPosition, 1.0f), mul(_19_uModelMatrix, mul(_19_uViewMatrix, _19_uProjectionMatrix)));
    gl_Position = float4(position.xy, position.z, position.w);
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
