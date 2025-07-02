#version 460 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texel;

out vec2 tex_chord;

uniform mat4 ortho;

void main() {
    tex_chord = texel;
    gl_Position = vec4(vertex,1.0,1.0) * ortho;
}