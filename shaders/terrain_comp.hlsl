#define TEX_SIZE 0.0625 // 1.0 / 16.0
#define TEX_INSERT 0.00130208 // 1.0 / 256.0 / 3.0

// The output vertex format
struct GeneratedVertex {
    float4 position;
    float2 uv;
    float luminance;
    uint border;
};

struct IndirectCommand {
    uint vertexCount;
    uint instanceCount;
    uint firstVertex;
    uint firstInstance;
};

// buffers
Texture2D<uint> InputTiles : register(t0, space0);
RWStructuredBuffer<GeneratedVertex> OutputVertices : register(u0, space1);
RWStructuredBuffer<IndirectCommand> IndirectDraw : register(u1, space1);

cbuffer Uniforms : register(b0, space2)
{
    float2 VIEW_SIZE;
}

void WriteVertex(uint index, float2 pos, float z, float2 uv, float lum, uint bdr) {
    GeneratedVertex v;
    v.position = float4(pos, z, 1.0);
    v.uv = uv;
    v.luminance = lum;
    v.border = bdr;
    OutputVertices[index] = v;
}

float GetLight(uint2 rPos){
    return float((InputTiles[rPos] >> 16) & 0xFu) / 15.0f;
}

void ComputeTile(uint2 rPos, float2 tPos){
    uint tile = InputTiles[rPos];

    uint front = tile & 0xFFu;
    uint back = (tile >> 8) & 0xFFu;

    if (!front && !back) return;

    uint border = (tile >> 20) & 0xFu;
    uint breakState = (tile >> 24) & 0xFu;

    // get vertex count
    uint vertices = 0;
    if (front) vertices += 6;
    if (back) vertices += 6;
    if (breakState) vertices += 6;

    uint index = 0;
    InterlockedAdd(IndirectDraw[0].vertexCount, vertices, index);

    // get neighbors light values
    float light[9];

    uint i = 0;
    // unsigned intergers wrap around if 0 - 1 so only max check is required
    for (int offsetY = -1; offsetY <= 1; ++offsetY)
    {
        for (int offsetX = -1; offsetX <= 1; ++offsetX)
        {
            uint x = rPos.x + offsetX;
            uint y = rPos.y + offsetY;

            if (x >= WIDTH || y >= HEIGHT)
            {
                // offscreen is 1.0 light
                light[i] = 1.0f;
            } else {
                light[i] = GetLight(uint2(x,y));
            }
            ++i;
        }
    }

    const float contrast = 2.0f;
    float BL = pow(((light[0] + light[1] + light[3] + light[4]) * 0.25f),contrast);
    float TL = pow(((light[3] + light[4] + light[6] + light[7]) * 0.25f),contrast);
    float BR = pow(((light[1] + light[2] + light[4] + light[5]) * 0.25f),contrast);
    float TR = pow(((light[4] + light[5] + light[7] + light[8]) * 0.25f),contrast);

    // Relative offsets
    const float2 offsets[4] = {
        tPos,                   // BL
        tPos + float2(BLOCK_SIZE, 0),          // BR
        tPos + float2(0, BLOCK_SIZE),          // TL
        tPos + float2(BLOCK_SIZE, BLOCK_SIZE)  // TR
    };

    // UV offsets
    const float2 uvOffsets[4] = {
        float2(TEX_INSERT, TEX_SIZE - TEX_INSERT),              // BL
        float2(TEX_SIZE - TEX_INSERT, TEX_SIZE - TEX_INSERT),   // BR
        float2(TEX_INSERT, TEX_INSERT),                         // TL
        float2(TEX_SIZE - TEX_INSERT, TEX_INSERT)               // TR
    };

    if (back)
    {
        float2 wallTexPos  = float2(float((back - 1) % 16) / 16.0,  float((back - 1) / 16) / 16.0);

        // Triangle 1: BL -> BR -> TL
        WriteVertex(index++, offsets[0], -1.0f, wallTexPos + uvOffsets[0], BL, border); // BL
        WriteVertex(index++, offsets[1], -1.0f, wallTexPos + uvOffsets[1], BR, border); // BR
        WriteVertex(index++, offsets[2], -1.0f, wallTexPos + uvOffsets[2], TL, border); // TL

        // Triangle 2: TL -> BR -> TR
        WriteVertex(index++, offsets[2], -1.0f, wallTexPos + uvOffsets[2], TL, border); // TL
        WriteVertex(index++, offsets[1], -1.0f, wallTexPos + uvOffsets[1], BR, border); // BR
        WriteVertex(index++, offsets[3], -1.0f, wallTexPos + uvOffsets[3], TR, border); // TR
    }
    if (front)
    {
        float2 blockTexPos = float2(float((front - 1) % 16) / 16.0, float((front - 1) / 16) / 16.0);

        // Triangle 1: BL -> BR -> TL
        WriteVertex(index++, offsets[0], 0, blockTexPos + uvOffsets[0], BL, border); // BL
        WriteVertex(index++, offsets[1], 0, blockTexPos + uvOffsets[1], BR, border); // BR
        WriteVertex(index++, offsets[2], 0, blockTexPos + uvOffsets[2], TL, border); // TL

        // Triangle 2: TL -> BR -> TR
        WriteVertex(index++, offsets[2], 0, blockTexPos + uvOffsets[2], TL, border); // TL
        WriteVertex(index++, offsets[1], 0, blockTexPos + uvOffsets[1], BR, border); // BR
        WriteVertex(index++, offsets[3], 0, blockTexPos + uvOffsets[3], TR, border); // TR
    }
    if (breakState)
    {
        float2 breakTexPos = float2(float((256 - breakState) % 16) / 16.0, float((256 - breakState) / 16) / 16.0);

        // Triangle 1: BL -> BR -> TL
        WriteVertex(index++, offsets[0], 1.0f, breakTexPos + uvOffsets[0], BL, border); // BL
        WriteVertex(index++, offsets[1], 1.0f, breakTexPos + uvOffsets[1], BR, border); // BR
        WriteVertex(index++, offsets[2], 1.0f, breakTexPos + uvOffsets[2], TL, border); // TL

        // Triangle 2: TL -> BR -> TR
        WriteVertex(index++, offsets[2], 1.0f, breakTexPos + uvOffsets[2], TL, border); // TL
        WriteVertex(index++, offsets[1], 1.0f, breakTexPos + uvOffsets[1], BR, border); // BR
        WriteVertex(index++, offsets[3], 1.0f, breakTexPos + uvOffsets[3], TR, border); // TR
    }
}

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float shiftY = (HEIGHT - CHUNK_SIZE) * 0.5f;
    float shiftX = (WIDTH - CHUNK_SIZE) * 0.5f;

    float limX = VIEW_SIZE.x + CHUNK_SIZE * BLOCK_SIZE;
    float limY = VIEW_SIZE.y + CHUNK_SIZE * BLOCK_SIZE;

    float2 position = float2(float(id.x) - shiftX,float(id.y) - shiftY) * BLOCK_SIZE;
    if (position.x < -(limX + BLOCK_SIZE) || position.x > limX ||
        position.y < -limY || position.y > limY)
    {
        return;
    }

    ComputeTile(id.xy,position);
}