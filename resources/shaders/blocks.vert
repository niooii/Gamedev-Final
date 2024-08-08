#version 450
#extension GL_ARB_gpu_shader_int64 : enable

layout(set = 0, binding = 0) uniform VertexUniformBuffer {
    mat4 view_projection;
} ubo;

layout(set = 1, binding = 0) readonly buffer FaceDataBuffer {
    uint64_t faces[];
} faceBuffer;

layout(push_constant) uniform PushConstants {
    int chunk_world_x;
    int chunk_world_y;
    int chunk_world_z;
} push_constants;

layout(location = 0) in vec3 up_plane_vertex;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_tex_coord;
layout(location = 2) out uint out_tex_id;

// TODO!
void main() {
    // out_tex_coord = vec2(in_position.x + 0.5, in_position.z + 0.5);
    // Extract data from the face data bits
    uint64_t one = 1;
    uint64_t three_bit_mask = ((one) << 3) - 1;
    uint64_t six_bit_mask = ((one) << 6) - 1;
    uint64_t eight_bit_mask = ((one) << 8) - 1;
    uint64_t face_data = faceBuffer.faces[gl_InstanceIndex];

    uint64_t direction = face_data & three_bit_mask;
    uint64_t block_pos_x = (face_data >> 3) & six_bit_mask;
    uint64_t block_pos_y = (face_data >> 9) & six_bit_mask;
    uint64_t block_pos_z = (face_data >> 15) & six_bit_mask;
    
    vec3 transformed_vertex = vec3(
        up_plane_vertex.x + float(block_pos_x), 
        up_plane_vertex.y + float(block_pos_y),
        up_plane_vertex.z + float(block_pos_z)
    );

    out_tex_id = uint((face_data >> 21) & eight_bit_mask);
    gl_Position = ubo.view_projection * vec4(transformed_vertex, 1.0);
}