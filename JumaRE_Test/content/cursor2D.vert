#version 460

layout(std140, binding = 0) uniform InternalModifierBlock
{
    vec2 uScreenCoordsModifier;
};
layout(std140, binding = 1) uniform ModifierBlock
{
    vec2 uLocation;
    vec2 uSize;
};

layout(location = 0) in vec2 vPosition;

void main()
{
    gl_Position = vec4((uLocation + vPosition * uSize) * uScreenCoordsModifier, 0.0f, 1.0f);
}