// Basic cube renderer

#version 450
#extension GL_ARB_gpu_shader_int64 : enable

layout(set = 0, binding = 0) uniform VertexUniformBuffer {
    mat4 view_projection;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    uint block_type;
} pc;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec2 out_tex_coord;
layout(location = 2) out uint block_type;

void main() {
    vec3 transformed_pos = in_position;
    transformed_pos.xyz += 0.5;
    out_tex_coord = vec2(in_position.x, in_position.z);
    block_type = pc.block_type;
    gl_Position = ubo.view_projection * pc.model * vec4(transformed_pos, 1);
}