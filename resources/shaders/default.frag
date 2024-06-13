#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec4 out_colour;

void main() {
    out_colour = vec4(sin(in_position.r), sin(in_position.g), sin(in_position.b),  1.0);
}