#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 16) out;

uniform mat4 ortho;

in uint _ui[];
in vec2 _position[];
in int _item[];
in int _stack[];

out vec2 texCoords;
flat out int atlas;

void main() {
    float UISize = 20.0f;
    float itemSize = 16.0f;
    float itemShift = 2.0f;
    float texSize = 0.2f;
    float itemTexSize = 0.0625f;
    vec2 numTexSize = vec2(0.03f,0.05f);

    vec2 quadVertices[4];
    vec2 texQuad[4];

    if (_ui[0] != 0u)
    {
        vec2 UITexPos = vec2(float((_ui[0] - 1) % 5) / 5.0f, float((_ui[0] - 1) / 5) / 5.0f);

        quadVertices = vec2[4](
            _position[0],                           // Bottom-left
            _position[0] + vec2(UISize, 0.0f),      // Bottom-right
            _position[0] + vec2(0.0f, UISize),      // Top-left
            _position[0] + vec2(UISize,UISize)      // Top-right
        );

        texQuad = vec2[4](
            UITexPos + vec2(0.0f, texSize),
            UITexPos + vec2(texSize, texSize),
            UITexPos,
            UITexPos + vec2(texSize, 0.0f)
        );

        // send UI to fragment shader
        atlas = 0;

        for (int v = 0; v < 4; v++) {
            texCoords = texQuad[v];
            gl_Position = vec4(quadVertices[v], 0.0f, 1.0f) * ortho;
            EmitVertex();
        }
        EndPrimitive();
    }

    // send item to fragment shader
    if (_item[0] == -1) return;

    uint item;
    if (_item[0] > 255)
    {
        atlas = 1;
        item = _item[0] - 256;
    } else
    {
        atlas = 2;
        item = _item[0];
    }

    vec2 ItemTexPos = vec2(float(item % 16) / 16.0f, float(item / 16) / 16.0f);

    texQuad = vec2[4](
        ItemTexPos + vec2(0.0f, itemTexSize),
        ItemTexPos + vec2(itemTexSize, itemTexSize),
        ItemTexPos,
        ItemTexPos + vec2(itemTexSize, 0.0f)
    );

    vec2 position = _position[0] + vec2(itemShift,itemShift);
    quadVertices = vec2[4](
        position,                   // Bottom-left
        position + vec2(itemSize, 0.0f),       // Bottom-right
        position + vec2(0.0f, itemSize),       // Top-left
        position + vec2(itemSize,itemSize)  // Top-right
    );

    for (int v = 0; v < 4; v++) {
        texCoords = texQuad[v];
        gl_Position = vec4(quadVertices[v], 0.0f, 1.0f) * ortho;
        EmitVertex();
    }
    EndPrimitive();

    // Render item count
    if (_stack[0] <= 1) return;
    atlas = 0;
    // allows numbers greater than 9
    int num = _stack[0];
    int i = 0;
    // limit to 2 digits since only 16 verticies out
    while (num > 0 && i != 2)
    {
        int digit = num % 10;

        float shift = 4.0f * float(i);

        // Calculate texture position
        vec2 NumTextPos = vec2(float(digit / 4) * numTexSize.x + 0.8f,float(digit % 4) * numTexSize.y + 0.8f);
        texQuad = vec2[4](
            NumTextPos + vec2(0.0f, numTexSize.y),
            NumTextPos + numTexSize,
            NumTextPos,
            NumTextPos + vec2(numTexSize.x, 0.0f)
        );
        // Caculate vertex position
        position = _position[0] + vec2(UISize - 5 - shift, itemShift);
        quadVertices = vec2[4](
            position,
            position + vec2(3.0f, 0.0f),
            position + vec2(0.0f, 4.0f),
            position + vec2(3.0f, 4.0f)
        );

        for (int v = 0; v < 4; v++) {
            texCoords = texQuad[v];
            gl_Position = vec4(quadVertices[v], 0.0f, 1.0f) * ortho;
            EmitVertex();
        }
        EndPrimitive();

        i++;
        num /= 10;
    }
}