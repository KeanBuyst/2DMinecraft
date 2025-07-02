#version 460 core

in vec2 tex_chord;

uniform sampler2D atlas;

void main() {
    gl_FragColor = texture(atlas,tex_chord);
}