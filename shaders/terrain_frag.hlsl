struct Input {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float luminance : COLOR0;
    nointerpolation uint border : COLOR1; 
};

Texture2D atlas : register(t0, space2);
SamplerState atlasSampler : register(s0, space2);

// Constants
static const uint TOP = 1;
static const uint RIGHT = 2;
static const uint BOTTOM = 4;
static const uint LEFT = 8;

float4 main(Input input) : SV_Target
{
    // 1. Sample Texture
    float4 color = atlas.Sample(atlasSampler, input.uv);
    if (color.a == 0.0) discard;

    float2 localPos = frac(input.uv * 16.0);
    float thickness = 0.08;

    // 2. Border Logic
    if (input.border != 0)
    {
        // Check strict boundaries (0.08 and 0.92) matching your GLSL
        bool isTop    = (input.border & TOP)    && (localPos.y <= thickness);
        bool isRight  = (input.border & RIGHT)  && (localPos.x >= (1.0 - thickness));
        bool isBottom = (input.border & BOTTOM) && (localPos.y >= (1.0 - thickness));
        bool isLeft   = (input.border & LEFT)   && (localPos.x <= thickness);

        if (isTop || isRight || isBottom || isLeft)
        {
            color.rgb = max(color.rgb - 0.2, 0.0);
        }
    }

    return float4(color.rgb * input.luminance, 1.0);
}