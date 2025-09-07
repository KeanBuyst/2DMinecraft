#version 430 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texel;
layout(location = 2) in float lightLevel;

out vec2 tex_chord;
out float light;

uniform mat4 ortho;

void main() {
    tex_chord = texel;
    light = lightLevel / 15.0f;
    gl_Position = vec4(vertex,1.0,1.0) * ortho;
}