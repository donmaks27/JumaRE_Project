static float4 gl_Position;
static float2 fPosition;
static float2 vPosition;

struct SPIRV_Cross_Input
{
    float2 vPosition : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float2 fPosition : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    fPosition = (vPosition / 2.0f.xx) + 0.5f.xx;
    gl_Position = float4(vPosition, 0.0f, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    vPosition = stage_input.vPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.fPosition = fPosition;
    return stage_output;
}
