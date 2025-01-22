#version 460

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push{
    mat2 transform;
    vec2 offset;
    vec3 color;
} push;

void main() {
    outColor = vec4(mix(
    2.0 * fragColor * push.color + fragColor * fragColor * (1.0 - 2.0 * push.color),
    sqrt(fragColor) * (2.0 * push.color - 1.0) + 2.0 * fragColor * (1.0 - push.color),
    step(0.5, push.color)) , 1.0);
}