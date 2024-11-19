#version 450

struct BlockTextureIds {
    uint top;
    uint bottom;
    uint left;
    uint right;
    uint front;
    uint back;
};

struct StaticBlockData {
    BlockTextureIds texture_ids;
};

layout(set = 1, binding = 0) uniform sampler2DArray block_textures;
layout(set = 1, binding = 1) readonly buffer StaticBlockDataBuffer {
    StaticBlockData table[];
} static_block_data;
layout(location = 0) in vec4 in_position;
layout(location = 1) in vec2 in_tex_coord;
layout(location = 2) flat in uint block_type;

layout(location = 0) out vec4 out_color;

void main() {
    BlockTextureIds tex_ids = static_block_data.table[block_type].texture_ids;
    out_color = texture(block_textures, vec3(in_tex_coord, tex_ids.top));
}