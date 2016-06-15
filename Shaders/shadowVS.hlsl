////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.vs
////////////////////////////////////////////////////////////////////////////////

Texture2D warpMaps          : register(t0);
SamplerState SampleTypeBorder : register(s0);
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
    float3  lightPosition;
    uint    rtwFlag;
    uint    sm_width;
	float3  padding;
};


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightDir : TEXCOORD2;
};

float GetWarp(float x, uint y) {
    if (x < 0.0f) return -0.025f;
    if (x > 1.0f) return 1.025f;
    float r, r1, r2;
    float fx = x * sm_width;
    uint x1 = floor(fx);
    uint x2 = x1 + 1;
    
    r1 = warpMaps[uint2(x1,y)].r;
    if (x2 >= sm_width - 1)
        r2 = 1.0f;
    else
        r2 = warpMaps[uint2(x2,y)].r;

    r = lerp(r1, r2, fx - x1); 
    return r * 2.0f - 1.0f;

}
float2 GetWarpUV(float2 uv) {
    uv.x = warpMaps.SampleLevel(SampleTypeBorder, float2(uv.x, .25f), 0).r;
    uv.y = warpMaps.SampleLevel(SampleTypeBorder, float2(uv.y, .75f), 0).r;
    return uv;
}
float2 GetWarpPos(float2 pos) {
    if (pos.x > 1.0f) return float2(1.025f, pos.y);
    if (pos.y > 1.0f) return float2(pos.x, 1.025f);
    if (pos.x < -1.0f) return float2(-1.025f, pos.y);
    if (pos.y < -1.0f) return float2(pos.x, -1.025f);
    float2 uv = pos.xy * float2(.5f, -.5f) + float2(.5f, .5f);
    return float2(GetWarp(uv.x, 0), -GetWarp(uv.y, 1));
}
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
    worldPosition = mul(input.position, worldMatrix);
    
    // 计算 点 在 相机下的投影
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 计算点在 light 下的投影
    output.lightViewPosition = mul(worldPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// 保存纹理坐标.
    output.tex = input.tex;
    
	// 计算 normal 向量
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    // 计算定点在世界坐标系下的位置

    // 计算出光照方向
    output.lightDir = lightPosition.xyz - worldPosition.xyz;
    output.lightDir = normalize(output.lightDir);

	return output;
}