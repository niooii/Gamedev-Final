#version 450

layout(set = 0, binding = 0) uniform VertexUniformBuffer {
    mat4 view_projection;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
} push_constants;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_position;

// TODO! this should KILL ITSELF.. Nlater.
layout(location = 1) out vec2 tex_coord;

void main() {
    tex_coord = vec2(in_position.x + 0.5, in_position.z + 0.5);
    // TODO! precompute this mvp matrix on the cpu
    gl_Position = ubo.view_projection * push_constants.model * vec4(in_position, 1.0);
}