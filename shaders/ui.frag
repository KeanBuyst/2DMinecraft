#version 430 core

uniform sampler2D uiAtlas;
uniform sampler2D tileAtlas;
uniform sampler2D itemAtlas;

flat in int atlas;
in vec2 texCoords;

out vec4 color;

void main() {
    switch (atlas)
    {
        case 0:
            color = texture(uiAtlas,texCoords);
            break;
        case 1:
            color = texture(tileAtlas,texCoords);
            break;
        case 2:
            color = texture(itemAtlas,texCoords);
            break;
    }
}