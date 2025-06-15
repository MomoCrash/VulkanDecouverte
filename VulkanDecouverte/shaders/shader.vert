#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 model;
} globalBuffer;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = globalBuffer.proj * globalBuffer.view * globalBuffer.model * vec4(position, 1.0);
    fragColor = vec4(255.0f, 255.0f, 255.0f, 255.0f);
}