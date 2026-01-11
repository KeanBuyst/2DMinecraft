struct Pixel {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float light : COLOR0;
};

Texture2D atlas : register(t0, space2);
SamplerState atlasSampler : register(s0, space2);

float4 main(Pixel pixel) : SV_Target
{
    float4 color = atlas.Sample(atlasSampler, pixel.uv);
    return float4(color.rgb * pixel.light, 1.0);
}