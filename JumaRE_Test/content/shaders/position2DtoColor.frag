#version 460

layout(location = 0) in vec2 fPosition;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fPosition.x, fPosition.y, 0.0f, 1.0f);
}