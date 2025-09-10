#version 430 core

in vec2 tex_chord;
in float light;

out vec4 color;

uniform sampler2D atlas;

void main() {
    color = texture(atlas,tex_chord);
    color = vec4(color.rgb * light, color.a);
}