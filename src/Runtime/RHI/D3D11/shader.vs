struct VSInput {
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};
struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(in VSInput input) {
    VSOutput output;
    output.pos = float4(input.pos, 1.0);
    output.uv = input.uv;
    return output;
}