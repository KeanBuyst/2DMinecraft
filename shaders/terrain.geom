#version 430 core

layout(points) in;    // expect one input point (base position)
layout(triangle_strip, max_vertices = 12) out;  // generate 1 or 2 quad (8 vertices)

in vec2 _position[];
in uint _block[];
in uint _wall[];
in uint _light[];
in uint _border[];

uniform mat4 ortho;

out vec2 texCoords; // Texture coordinates for the fragment shader
out vec2 position;
flat out float lumeninance;
flat out uint border;

void main() {
    vec2 blockSize = vec2(16,16);
    vec2 texSize = vec2(0.0625,0.0625);

    if (_block[0] == 0 && _wall[0] == 0) return;
    lumeninance = float(_light[0]) / 15.0;

    border = _border[0];

    vec2 blockTexPos = vec2(float((_block[0] - 1) % 16) / 16.0,float((_block[0] - 1) / 16) / 16.0);
    vec2 wallTexPos = vec2(float((_wall[0] - 1) % 16) / 16.0,float((_wall[0] - 1) / 16) / 16.0);

    // Generate the quad for this block (2 triangles = 4 vertices)
    vec2 quadVertices[4] = vec2[4](
        _position[0],                       // Bottom-left
        _position[0] + vec2(blockSize.x, 0), // Bottom-right
        _position[0] + vec2(0, blockSize.y), // Top-left
        _position[0] + blockSize             // Top-right
    );

    vec2 positions[4] = vec2[4](
        vec2(0.0,0.0),
        vec2(1.0,0.0),
        vec2(0.0,1.0),
        vec2(1.0,1.0)
    );

    if (_wall[0] != 0){
        vec2 quadWallTexCoords[4] = vec2[4](
            wallTexPos + vec2(0.0, texSize.y),  // Bottom-left
            wallTexPos + vec2(texSize.x, texSize.y), // Bottom-right
            wallTexPos,                         // Top-left
            wallTexPos + vec2(texSize.x, 0.0)   // Top-right
        );
        // Emit the vertices for the block quad
        for (int v = 0; v < 4; v++) {
            texCoords = quadWallTexCoords[v];
            gl_Position = vec4(quadVertices[v], -1.0, 1.0) * ortho;  // Set the final position
            EmitVertex();
        }

        EndPrimitive();  // End the current quad
    }

    if (_block[0] != 0){
        vec2 quadBlockTexCoords[4] = vec2[4](
            blockTexPos + vec2(0.0, texSize.y),  // Bottom-left
            blockTexPos + vec2(texSize.x, texSize.y), // Bottom-right
            blockTexPos,                         // Top-left
            blockTexPos + vec2(texSize.x, 0.0)   // Top-right
        );
        // Emit the vertices for the block quad
        for (int v = 0; v < 4; v++) {
            texCoords = quadBlockTexCoords[v];
            position = positions[v];
            gl_Position = vec4(quadVertices[v], 0.0, 1.0) * ortho;  // Set the final position
            EmitVertex();
        }

        EndPrimitive();  // End the current quad
    }
}