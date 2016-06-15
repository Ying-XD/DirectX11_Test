////////////////////////////////////////////////////////////////////////////////
// Filename: depth.ps
////////////////////////////////////////////////////////////////////////////////

Texture2D ShadowTexture       : register(t0);

SamplerState SampleClamp        : register(s0);
struct PixelInputType
{
    float4  pos         : SV_POSITION;
    float4  depthPos    : TEXTURE0;
    float4  normal      : NORMAL;
    float4  viewDir     : VIEW_VECOTR;
};

struct ShadowDepPSType {
    float4  pos         : SV_POSITION;
    float4  lightPos    : TEXCOORD0;
    float4  dptPos      : TEXCOORD1;
    float3  normal      : NORMAL;
    float3  lightDir    : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float DepthPixelShader(PixelInputType input) : SV_TARGET
{
	float depthValue;

    // 获得深度，深度是 w 齐次 之后的值
	depthValue = input.depthPos.z / input.depthPos.w;

	return depthValue;
}

float NVPixelShader(PixelInputType input) : SV_TARGET {
    float nvValue = dot(input.viewDir , input.normal);
    nvValue = clamp(-nvValue,0,1);

    return nvValue;
}

float ShadowDepPS(ShadowDepPSType input) : SV_TARGET{
    float   bias = 0.0001f;
    float   dpt = 1.0f;
    float2  lightProjPos;
    float   lightDepValue;
    float   depValue;
    lightProjPos = input.lightPos.xy / input.lightPos.w * float2(0.5f, -0.5f) + float2(0.5f,0.5f);

    depValue = ShadowTexture.Sample(SampleClamp, lightProjPos).r;

    
    lightDepValue = input.lightPos.z / input.lightPos.w;

    lightDepValue -= bias;
    float lightIntensity = dot(input.normal, input.lightDir);



    //if (lightDepValue <= depValue )
    //    return 1.5f;
    //else
    //    return input.dptPos.z / input.dptPos.w;          // shadow

    dpt = 1.5f;
    if (lightDepValue <= depValue || lightIntensity < 0.1f)
        return 1.5f;
    else
        dpt = input.dptPos.z / input.dptPos.w;          // shadow

    return dpt;

}