#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} globalBuffer;

layout (binding = 1) uniform UboInstance {
    mat4 model;
} uboInstance;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = globalBuffer.proj * globalBuffer.view * uboInstance.model * vec4(position, 1.0);
    fragColor = vec4(normal.x, normal.y, normal.z, 255.0f);
    fragTexCoord = texCoords;
}