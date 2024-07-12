#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view_projection;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
} push_constants;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_position;

// vec2 positions[3] = vec2[](
//     vec2(0.0, -0.5),
//     vec2(0.5, 0.5),
//     vec2(-0.5, 0.5)
// );

void main() {
    // TODO! precompute this mvp matrix on the cpu
    gl_Position = ubo.view_projection * push_constants.model * vec4(in_position, 1.0);
}