#version 460

layout(location = 0) in vec2 vPosition;

layout(location = 0) out vec2 fPosition;

void main()
{
    fPosition = vPosition / 2 + 0.5f;
    gl_Position = vec4(vPosition, 0, 1);
}