struct VSIn
{
    float3 position : POSITION;
    float3 color : COLOR;
};
struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
cbuffer transformations : register(b0)
{
    matrix MVP;
}

VSOut main( VSIn input )
{
    VSOut output;
    output.position = mul(MVP, float4(input.position, 1.0f));
    output.color = float4(input.color, 1.0f);
    return output;
}