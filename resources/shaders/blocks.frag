#version 450

struct BlockTextureIds {
    uint top;
    uint bottom;
    uint left;
    uint right;
    uint front;
    uint back;
};


layout(set = 1, binding = 0) uniform sampler2DArray block_textures;
layout(set = 1, binding = 1) readonly buffer StaticBlockDataBuffer {
    BlockTextureIds texture_ids[];
} static_block_data;
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coord;
layout(location = 2) flat in uint block_type;

layout(location = 0) out vec4 out_color;

void main() {
    BlockTextureIds tex_ids = static_block_data.texture_ids[block_type];
    out_color = texture(block_textures, vec3(in_tex_coord, tex_ids.top));
}