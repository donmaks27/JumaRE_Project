#version 460

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTextureCoords;

layout(location = 0) out vec2 fTextureCoords;

void main()
{
    fTextureCoords = vTextureCoords;
    gl_Position = vec4(vPosition, 0.0f, 1.0f);
}