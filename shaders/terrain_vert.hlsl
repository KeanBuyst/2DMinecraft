struct VertexInput {
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float luminance : TEXCOORD1;
    uint border : TEXCOORD2;
};

struct VertexOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float luminance : COLOR0;
    nointerpolation uint border : COLOR1;
};

cbuffer Uniforms : register(b0, space1)
{
    float4x4 ortho;
    float2 origin;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;

    const float SIZE = CHUNK_SIZE * BLOCK_SIZE;

    float2 relOrigin = fmod(origin,SIZE);
    if (relOrigin.x < 0) relOrigin.x += SIZE;
    if (relOrigin.y < 0) relOrigin.y += SIZE;

    float4 pos = float4(input.position.xy - relOrigin,input.position.zw);
    output.position = mul(ortho,pos);
    output.uv = input.uv;
    output.luminance = input.luminance;
    output.border = input.border;
    
    return output;
}