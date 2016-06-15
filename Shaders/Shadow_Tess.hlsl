Texture2D warpMaps          : register(t0);
Texture2D shaderTexture     : register(t1);
Texture2D shadowTexture     : register(t2);

SamplerState SampleTypeWrap  : register(s0);
SamplerState SampleTypeClamp : register(s1);
SamplerState SampleTypeBorder : register(s2);
// ----------   Const Buffer    ----------
cbuffer TessBuffer : register(b0){
    float   tessAmount;
};

cbuffer MatrixBuffer : register(b1){
    matrix  worldMatr;
    matrix  viewMatr;
    matrix  projMatr;
    matrix  lightViewMatr;
	matrix  lightProjMatr;
    float3  lightPosition;
    uint    sm_width;
};

cbuffer LightBuffer: register(b2) {
    float4 ambientColor;
	float4 diffuseColor;
};

// -----    struct  -----
struct VS_INPUT {
    float3  pos         : POSITION;
    float2  uv          : TEXCOORD0;
    float3  normal      : NORMAL;
};

struct HS_INPUT {
    float3 pos          : POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL;
};

struct PatchTess{
    float Edges[3]      : SV_TessFactor;
    float Inside        : SV_InsideTessFactor;
};

struct DS_INPUT {
    float3 pos          : POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL;
};

struct PS_INPUT {
    float4 vPos         : SV_POSITION;
    float2 uv           : TEXCOORD0;
	float3 normal       : NORMAL;
    float4 lightPos     : TEXCOORD1;
	float3 lightDir     : TEXCOORD2;
};

float2 GetWarpUV(float2 uv) {
    uv.x = warpMaps.Sample(SampleTypeBorder, float2(uv.x, .25f)).r;
    uv.y = warpMaps.Sample(SampleTypeBorder, float2(uv.y, .75f)).r;
    return uv;
}
// ----------   Vertex Shader   ----------
HS_INPUT ShadowVS(VS_INPUT input) {
    HS_INPUT output;
    output.pos = input.pos;
    output.uv = input.uv;
    output.normal = input.normal;
    return output;
}

// ----------   Hull Shader     ----------
PatchTess ConstantHS(InputPatch<HS_INPUT, 3> iPatch, uint patchId : SV_PrimitiveID) {
    PatchTess output;
    output.Edges[0] = tessAmount;
    output.Edges[1] = tessAmount;
    output.Edges[2] = tessAmount;
    output.Inside   = tessAmount;

    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
DS_INPUT ShadowHS(InputPatch<HS_INPUT, 3> iPatch,
                uint pointID : SV_OutputControlPointID,
                uint patchID : SV_PrimitiveID
) {
    DS_INPUT output;
    output.pos = iPatch[pointID].pos;
    output.uv = iPatch[pointID].uv;
    output.normal = iPatch[pointID].normal;

    return output;
}

// ----------   Domain Shader   ----------
[domain("tri")]
PS_INPUT ShadowDS(PatchTess input, 
                float3 uvwCoord : SV_DomainLocation,
                const OutputPatch<DS_INPUT, 3> patch
) {
    PS_INPUT output;
    float4  vpos;
    vpos.xyz = uvwCoord.x * patch[0].pos + uvwCoord.y* patch[1].pos + uvwCoord.z * patch[2].pos;
    vpos.w = 1.0f;
    output.uv = uvwCoord.x * patch[0].uv + uvwCoord.y* patch[1].uv + uvwCoord.z * patch[2].uv;
    output.normal = uvwCoord.x * patch[0].normal  + uvwCoord.y* patch[1].normal  + uvwCoord.z * patch[2].normal ;

    vpos = mul(vpos, worldMatr);

    output.vPos = mul(vpos, viewMatr);
    output.vPos = mul(output.vPos , projMatr);
    
    output.lightPos = mul(vpos, lightViewMatr);
    output.lightPos = mul(output.lightPos, lightProjMatr);

    output.normal = mul(output.normal, (float3x3)worldMatr);
    output.normal = normalize(output.normal);

    output.lightDir = lightPosition.xyz - vpos.xyz;
    output.lightDir = normalize(output.lightDir);

    return output;
}


// ----------   Pixel Shader   ----------
float4 ShadowPS(PS_INPUT input) : SV_Target {
    float bias = 0.001f;
    float4 color = ambientColor;
    float lightIntensity;

    float2 rtwCoord = input.lightPos.xy / input.lightPos.w * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    rtwCoord = GetWarpUV(rtwCoord);

    if (saturate(rtwCoord.x) != rtwCoord.x || saturate(rtwCoord.y) != rtwCoord.y) {
        lightIntensity = saturate(dot(input.normal, input.lightDir));
        if (lightIntensity > 0.0f) {
            color += (diffuseColor * lightIntensity);
            color = saturate(color);
        }
    }
    else {
        float depValue = shadowTexture.Sample(SampleTypeClamp, rtwCoord).r;
        float lightDepValue = input.lightPos.z / input.lightPos.w;

        lightDepValue -= bias;
        if (lightDepValue <= depValue) {
            lightIntensity = saturate(dot(input.normal, input.lightDir));
            if(lightIntensity > 0.0f) {
			    color += (diffuseColor * lightIntensity);
			    color = saturate(color);
		    }
        }
    }

    float4 texColor = shaderTexture.Sample(SampleTypeWrap, input.uv);
    color = color * texColor;
    return color;
}
