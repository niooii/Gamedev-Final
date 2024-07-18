#version 450

layout(set = 0, binding = 1) uniform sampler2DArray blockTextures;
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coord;

layout(location = 0) out vec4 out_color;

void main() {
    const uint fakeTexId = 0;
    out_color = texture(blockTextures, vec3(in_tex_coord, fakeTexId));
}