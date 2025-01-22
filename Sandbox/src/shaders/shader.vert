#version 460

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push{
    mat2 transform;
    vec2 offset;
    vec3 color;
} push;

void main() {
    gl_Position = (ubo.proj * ubo.view * ubo.model * vec4(push.transform * inPosition + push.offset, 0.0, 1.0));
    fragColor = inColor;
}