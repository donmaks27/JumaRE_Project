#version 460

layout(std140, binding = 0) uniform ModifierBlock
{
    vec2 uLocation;
    vec2 uSize;
};

layout(location = 0) in vec2 vPosition;

void main()
{
    gl_Position = vec4(uLocation + vPosition * uSize, 0.0f, 1.0f);
}