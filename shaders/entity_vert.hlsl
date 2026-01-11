struct InputVertex {
    float2 position : POSITION0;
    float2 uv : TEXCOORD0;
    float light: TEXCOORD1;
};

struct OutputVertex {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float light : COLOR0;
};


cbuffer Uniforms : register(b0, space1)
{
    float4x4 ortho;
};

OutputVertex main(InputVertex vertex)
{
    OutputVertex output;
    output.position = mul(ortho,float4(vertex.position,0.0f,1.0f));
    output.uv = vertex.uv;
    output.light = vertex.light / 15.0f;
    return output;
}