#version 460

layout(location = 0) in vec2 fTextureCoords;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fTextureCoords.x, fTextureCoords.y, 0.0f, 1.0f);
}