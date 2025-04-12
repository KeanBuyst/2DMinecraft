#version 460 core

in vec2 texCoords;
in float lumeninance;

uniform sampler2D atlas;

void main() {
    vec4 color = texture(atlas,texCoords);

    if (color.r == 0.0 && color.g == 0.0 && color.b == 0.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    } else {
        gl_FragColor = vec4(color.rgb * lumeninance, 1.0);
    }
}
