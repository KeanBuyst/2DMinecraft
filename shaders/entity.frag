#version 460 core

in vec2 tex_chord;
in float light;

uniform sampler2D atlas;

void main() {
    vec4 color = texture(atlas,tex_chord);
    gl_FragColor = vec4(color.rgb * light, 1.0);
}