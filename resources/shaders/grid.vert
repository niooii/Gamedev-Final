#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view_projection;
} ubo;

layout(push_constant) uniform PushConstants {
    vec3 camera_pos;
} pc;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_world_pos;

layout(location = 1) out vec3 camera_pos;

void main() {
    camera_pos = pc.camera_pos;
    // grid wont have a world transform, scale it here
    vec3 transformed_vertex = in_position * 40;
    transformed_vertex.xz += pc.camera_pos.xz;
    // // because the input is the "up plane" for a cube so set y to 0.
    transformed_vertex.y = -0.5;

    out_world_pos = transformed_vertex;
    gl_Position = ubo.view_projection * vec4(transformed_vertex, 1.0);
    // gl_Position = vec4(out_position, 1.0);
}