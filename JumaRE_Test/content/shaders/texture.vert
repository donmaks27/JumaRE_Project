#version 460

layout(std140, binding = 0) uniform CoordModifierBlock
{
    vec2 uScreenCoordsModifier;
};

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTextureCoords;

layout(location = 0) out vec2 fTextureCoords;

void main()
{
    fTextureCoords = vTextureCoords;
    gl_Position = vec4(vPosition * uScreenCoordsModifier, 0.0f, 1.0f);
}