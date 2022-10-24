cbuffer ModifierBlock : register(b1)
{
    float2 _19_uLocation : packoffset(c0);
    float2 _19_uSize : packoffset(c0.z);
};

cbuffer InternalModifierBlock : register(b0)
{
    float2 _33_uScreenCoordsModifier : packoffset(c0);
};


static float4 gl_Position;
static float2 vPosition;

struct SPIRV_Cross_Input
{
    float2 vPosition : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    gl_Position = float4((_19_uLocation + (vPosition * _19_uSize)) * _33_uScreenCoordsModifier, 0.0f, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    vPosition = stage_input.vPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    return stage_output;
}
