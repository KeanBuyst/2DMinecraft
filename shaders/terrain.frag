#version 460 core

in vec2 texCoords;
in vec2 position;
flat in float lumeninance;
flat in uint border;

uniform sampler2D atlas;

// in pixels
float borderWidth = 1.0f;

uint TOP = 1;
uint RIGHT = 2;
uint BOTTOM = 4;
uint LEFT = 8;

bool isEmpty(vec4 color)
{
    return color.rgb == vec3(0.0,0.0,0.0);
}

void main() {

    vec4 color = texture(atlas,texCoords);

    if (border != 0)
    {
        if (!isEmpty(color) && (
            (bool(border & TOP) && position.y >= 0.92) ||
            (bool(border & RIGHT) && position.x >= 0.92) ||
            (bool(border & BOTTOM) && position.y <= 0.08) ||
            (bool(border & LEFT) && position.x <= 0.08)))
        {
            color -= vec4(0.2, 0.2, 0.2, 0.0);
        }
    }

    if (isEmpty(color)) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    } else {
        gl_FragColor = vec4(color.rgb * lumeninance, 1.0);
    }
}
