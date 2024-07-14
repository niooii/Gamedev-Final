#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view_projection;
} ubo;

layout(push_constant) uniform PushConstants {
    vec3 camera_pos;
} pc;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_position;

layout(location = 1) out vec3 out_world_pos;

layout(location = 2) out vec3 camera_pos;

void main() {
    // grid wont have a world transform, scale it here
    vec3 transformed_vertex = in_position * 20;
    transformed_vertex.xz += pc.camera_pos.xz;
    out_world_pos.xz = transformed_vertex.xz;
    camera_pos = pc.camera_pos;

    gl_Position = ubo.view_projection * vec4(transformed_vertex, 1.0);
}