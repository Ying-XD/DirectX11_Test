// Filename: DeferredVS.hlsl


cbuffer MatrixBuffer {
	matrix  worldMatr;
	matrix  viewMatr;
	matrix  projMatr;
    float3  lightPos;
    float   padding;
};
cbuffer LightBuffer {
    matrix  lightViewMatrix;
    matrix  lightPorjMatrtix;
};
// -- struct --
struct VSInput {
    float4  pos         : POSITION;
    float2  uv          : TEXCOORD0; 
    float3  normal      : NORMAL; 
};

struct PSInput {
    float4  pos         : SV_Position;
    float2  uv          : TEXCOORD0;
    float3  normal      : NORMAL; 
    float3  lightDir    : TEXCOORD1;
    float4  dpt         : TEXCOORD2;
    float4  lightPos    : TEXCOORD3;
};

PSInput DefferedVS(VSInput input) {
    PSInput output;

    input.pos.w = 1.0f;
    output.pos = mul(input.pos, worldMatr);
    output.pos = mul(output.pos, viewMatr);
    output.pos = mul(output.pos, projMatr);

    float4 worldPos = mul(input.pos, worldMatr);

    output.normal = mul(input.normal, (float3x3)worldMatr);
    output.normal = normalize(output.normal);

    output.lightDir = lightPos.xyz - worldPos.xyz;
    output.lightDir = normalize(output.lightDir);

    output.uv = input.uv;

    output.dpt = input.pos;

    output.lightPos = mul(worldPos, lightViewMatrix);
    output.lightPos = mul(output.lightPos, lightPorjMatrtix);
    
    return output;
}

