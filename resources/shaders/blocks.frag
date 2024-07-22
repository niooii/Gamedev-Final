#version 450

layout(set = 0, binding = 1) uniform sampler2DArray blockTextures;
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coord;
layout(location = 2) flat in uint in_tex_id;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(blockTextures, vec3(in_tex_coord, in_tex_id));
}