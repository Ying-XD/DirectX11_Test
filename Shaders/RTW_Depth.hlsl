// -----    RTW_DepthVS.hlsl    ----
Texture2D warpMaps      : register(t0);
SamplerState SampleTypeBorder  : register(s0);

cbuffer ConstBuffer {
    matrix  worldMatrix;
    matrix  viewMatrix;
    matrix  projMatrix;
};

struct VertexInputType {
    float4  pos         : POSITION;
    float2  tex         : TEXCOORD;
    float3  normal      : NORMAL;
};

struct PixelInputType {
    float4  pos             : SV_POSITION;
    float4  depthPos        : COLOR0;
};

float2 GetWarpPos(float2 uv) {
    if (uv.x > 1.0f)
        uv.x = 1.025f;
    else if (uv.x < 0.0f)
        uv.x = -0.025f;
    else
        uv.x += warpMaps.SampleLevel(SampleTypeBorder, float2(uv.x, .25f), 0).r;

    if (uv.y > 1.0f)
        uv.y = -0.025f;
    else if (uv.y < 0.0f)
        uv.y = 1.025f;
    else
        uv.y += warpMaps.SampleLevel(SampleTypeBorder, float2(uv.y, .75f), 0).r;

    return uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
}
PixelInputType RTW_DepthVertexShader(VertexInputType input ) {
    matrix wvpMat = mul(worldMatrix, viewMatrix);
    wvpMat = mul(wvpMat, projMatrix);

    PixelInputType output;
    input.pos.w = 1.0f;
    output.pos = mul(input.pos, wvpMat);
    output.depthPos = output.pos;
   
    float u = output.pos.x / output.pos.w * 0.5f + 0.5f;
    float v = -output.pos.y / output.pos.w * 0.5f + 0.5f;
    output.pos.xy = GetWarpPos(float2(u, v)) * output.pos.w;
    return output;
}

float RTW_DepthPixelShader(PixelInputType input) :SV_TARGET{
    return (input.depthPos.z / input.depthPos.w);
}


