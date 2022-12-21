#version 460

layout(std140, binding = 0) uniform ModifierBlock
{
    vec2 uLocation;
    vec2 uSize;
    vec2 uOffset;
    float uDepth;
};

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTexCoords;

void main()
{
    gl_Position = vec4(uLocation + uSize * (uOffset + vPosition), uDepth, 1.0f);
}