#version 450

layout(std140, binding = 0) uniform ModifierBlock
{
    mat4 uModelMatrix;
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
};

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureCoords;

layout(location = 0) out vec2 fTextureCoords;

void main()
{
    fTextureCoords = vTextureCoords;
    vec4 position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(vPosition, 1.0f);
    gl_Position = vec4(position.xy, position.z, position.w);
}