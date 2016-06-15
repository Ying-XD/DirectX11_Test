// Filename: DeferredPS.hlsl
Texture2D       shaderTex   : register(t0);
Texture2D       shadowTex   : register(t1);

SamplerState    SamplerWarp : register(s0);

cbuffer LightBuffer {
	float4 ambientColor;
	float4 diffuseColor;
};

struct PSInput {
    float4  pos         : SV_Position;
    float2  uv          : TEXCOORD0;
    float3  normal      : NORMAL; 
    float3  lightDir    : TEXCOORD1;
    float4  dpt         : TEXCOORD2;
    float4  lightPos    : TEXCOORD3;

};
struct PSOutput {
    float4  color           : SV_Target0;
    float4  diffuseColor    :SV_Targe1;
};

struct PSOutput2 {
    float3  normal  : SV_Target0;
    float   dpt     : SV_Target1;
    float   lightDpt: SV_Target2;
};
float4 DefferedPS0(PSInput input) : SV_TARGET{
    float4 texColor = shaderTex.Sample(SamplerWarp, input.uv);
    
    float lightIntensity = saturate(dot(input.normal, input.lightDir));

    float4 color = diffuseColor * lightIntensity + ambientColor;
    color *= texColor;
    return color;
}

PSOutput DefferedPS(PSInput input) : SV_TARGET {
    PSOutput output;
    output.color = shaderTex.Sample(SamplerWarp, input.uv);// * ambientColor;
    float   f = dot(input.normal, input.lightDir);
    output.diffuseColor = dot(input.normal, input.lightDir) * diffuseColor;

    float4 texColor = shaderTex.Sample(SamplerWarp, input.uv);
    
    float lightIntensity = saturate(dot(input.normal, input.lightDir));

    float4 color = diffuseColor * lightIntensity + ambientColor;
    color *= texColor;
    //output.color = color;
    
    return output;
}

PSOutput2 DefferedPS_Multiple2(PSInput input) : SV_TARGET {
    PSOutput2 output;
    output.normal = input.normal;
    output.dpt = input.dpt.z / input.dpt.w;
    
    float   bias = 0.001f;
    float2  uv = input.lightPos.xy / input.lightPos.w * float2(.5f, -.5f) + float2(.5f, .5f);
    float   lightDep = input.lightPos.z / input.lightPos.w;
    float   depValue = shadowTex.Sample(SamplerWarp, uv).r;
    float   lightIntensity = dot(input.normal, input.lightDir);
    if (lightDep <= depValue || lightIntensity < 0.1f)
        output.lightDpt = 1.5f;
    else
        output.lightDpt = output.dpt;

    return output;
}