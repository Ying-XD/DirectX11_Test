//  --------------- shadowVS.hlsl ---------------

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


cbuffer LightBuffer
{
    float3  lightPosition;
    uint    rtwFlag;
    uint    sm_width;
	float3  padding;
};


struct VS_INPUT
{
    float4 position : POSITION;
    float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
    float3 lightDir : TEXCOORD1;
};


PS_INPUT ShadowVertexShader(VS_INPUT input)
{
    PS_INPUT output;
	float4 worldPosition;

    input.position.w = 1.0f;
    worldPosition = mul(input.position, worldMatrix);
    
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    output.lightDir = lightPosition.xyz - worldPosition.xyz;
    output.lightDir = normalize(output.lightDir);

	return output;
}