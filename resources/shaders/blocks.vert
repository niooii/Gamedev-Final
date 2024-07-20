#version 450
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : enable

layout(set = 0, binding = 0) uniform VertexUniformBuffer {
    mat4 view_projection;
} ubo;

// layout(push_constant) uniform PushConstants {
//     uint64_t face_data;
// } push_constants;

layout(location = 0) in uint64_t face_data;

layout(location = 0) out vec3 out_position;

// TODO! this should KILL ITSELF.. Nlater.
layout(location = 1) out vec2 out_tex_coord;
layout(location = 2) out uint out_tex_id;

void main() {
    out_tex_coord = vec2(in_position.x + 0.5, in_position.z + 0.5);
    // Extract data from the face data bits
    uint three_bit_mask = (((uint)1) << 3) - 1;
    uint six_bit_mask = (((uint)1) << 6) - 1;
    uint eight_bit_mask = (((uint)1) << 8) - 1;

    uint direction = face_data & three_bit_mask;
    uint block_pos_x = (face_data >> 3) & six_bit_mask;
    uint block_pos_y = (face_data >> 9) & six_bit_mask;
    uint block_pos_z = (face_data >> 15) & six_bit_mask;
    out_tex_id = (face_data >> 21) & eight_bit_mask;
    gl_Position = ubo.view_projection * push_constants.model * vec4(in_position, 1.0);
}