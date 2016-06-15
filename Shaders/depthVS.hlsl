// Filename: depthVS.hlsl


cbuffer MatrixBuffer : register(c0)
{
	matrix  worldMatrix;
	matrix  viewMatrix;
	matrix  projectionMatrix;
    matrix  lightViewMatrix;
	matrix  lightProjMatrix;

    
};
cbuffer PositonBuffer : register(c1){
    float3  cameraPos;
    float   padding;
    float3  lightPos;
    float1  padding2;
};

// --- Struct   ----
struct VertexInputType
{
    float4  pos     : POSITION;
    float2  tex     : TEXCOORD0;
	float3  normal  : NORMAL;
};

struct PixelInputType
{
    float4  pos         : SV_POSITION;
    float4  depthPos    : TEXTURE0;
    float4  normal      : NORMAL;
    float4  viewDir     : POSTION;
};

struct ShadowDepPSType {
    float4  pos         : SV_POSITION;
    float4  lightPos    : TEXCOORD0;
    float4  dptPos      : TEXCOORD1;
    float3  normal      : NORMAL;
    float3  lightDir    : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType DepthVertexShader(VertexInputType input) {
    PixelInputType output;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.pos.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.normal = mul(float4(input.normal,0), worldMatrix);
    output.normal = normalize(output.normal);
	
    output.viewDir = float4(normalize(input.pos.xyz - cameraPos),0);

    
    // Store the position value in a second input value for depth value calculations.

	output.depthPos = output.pos;

	return output;
}

ShadowDepPSType ShadowDepVS(VertexInputType input) {
    ShadowDepPSType output;
    input.pos.w = 1.0f;
    
    float4  worldPos = mul(input.pos, worldMatrix);
    
    output.pos = mul(worldPos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.lightPos = mul(worldPos, lightViewMatrix);
    output.lightPos = mul(output.lightPos, lightProjMatrix);

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    output.lightDir = lightPos.xyz - worldPos.xyz;
    output.lightDir = normalize(output.lightDir);
   
   
    output.dptPos = output.pos;
    

    return output;
}