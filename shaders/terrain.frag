#version 460 core

out vec4 color;

in vec2 texCoords;
in float lumeninance;

uniform sampler2D atlas;

void main() {
    color = texture(atlas,texCoords) * lumeninance;
}
