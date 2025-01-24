#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_NV_gpu_shader5 : enable

layout(points) in;    // We expect one input point (base position)
layout(triangle_strip, max_vertices = 32) out;  // We generate 8 quads (4 vertices each)

in vec2 rowPos[];
in uint64_t blocks[];
in uint64_t walls[];
in uint lightMap[];

uniform mat4 ortho;

out vec2 texCoords; // Texture coordinates for the fragment shader
out float lumeninance;

void main() {
	vec2 blockSize = vec2(16,16);
    vec2 texSize = vec2(0.0625,0.0625);

	// Unpack integers into 8 blocks
    for (uint i = 0; i < 8; i++) {
		uint block = uint(((blocks[0] >> (i * 8)) & 0xFFu));
        uint wall = uint(((walls[0] >> (i * 8)) & 0xFFu));
        if (block == 0 && wall == 0) continue;
        lumeninance = float((lightMap[0] >> (i * 4)) & 0xFu) / 15.0;

        vec2 blockPos = rowPos[0] + vec2(0.0, blockSize - (i * blockSize.y));

        vec2 blockTexPos = vec2(float((block % 16) - 1) / 16.0,float(block / 16) / 16.0);
        vec2 wallTexPos = vec2(float((wall % 16) - 1) / 16.0,float(wall / 16) / 16.0);

		// Generate the quad for this block (2 triangles = 4 vertices)
        vec2 quadVertices[4] = vec2[4](
            blockPos,                       // Bottom-left
            blockPos + vec2(blockSize.x, 0), // Bottom-right
            blockPos + vec2(0, blockSize.y), // Top-left
            blockPos + blockSize             // Top-right
        );

        vec2 quadBlockTexCoords[4] = vec2[4](
            blockTexPos + vec2(0.0, texSize.y),  // Bottom-left
            blockTexPos + vec2(texSize.x, texSize.y), // Bottom-right
            blockTexPos,                         // Top-left
            blockTexPos + vec2(texSize.x, 0.0)   // Top-right
        );

        vec2 quadWallTexCoords[4] = vec2[4](
            wallTexPos + vec2(0.0, texSize.y),  // Bottom-left
            wallTexPos + vec2(texSize.x, texSize.y), // Bottom-right
            wallTexPos,                         // Top-left
            wallTexPos + vec2(texSize.x, 0.0)   // Top-right
        );


        if (block != 0){
            // Emit the vertices for the block quad
            for (int v = 0; v < 4; v++) {
                texCoords = quadBlockTexCoords[v];
                gl_Position = vec4(quadVertices[v], 0.0, 1.0) * ortho;  // Set the final position
                EmitVertex();
            }

            EndPrimitive();  // End the current quad
        }

        if (wall != 0){
            // Emit the vertices for the block quad
            for (int v = 0; v < 4; v++) {
                texCoords = quadWallTexCoords[v];
                gl_Position = vec4(quadVertices[v], -1.0, 1.0) * ortho;  // Set the final position
                EmitVertex();
            }

            EndPrimitive();  // End the current quad
        }
	}
}