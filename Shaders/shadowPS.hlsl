//  --------------- shadowPS.hlsl ---------------

Texture2D shaderTexture     : register(t0);

SamplerState SampleTypeWrap  : register(s0);
SamplerState SampleTypeClamp : register(s1);
SamplerState SampleTypeBorder : register(s2);

cbuffer LightBuffer {
	float4  ambientColor;
	float4  diffuseColor;
};


struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 lightDir : TEXCOORD1;
};


float4 ShadowPixelShader(PS_INPUT input) : SV_TARGET
{
    float4  color;
    float   lightIntensity;
	float4  textureColor;
   
    color = ambientColor;
    
    lightIntensity = saturate(dot(input.normal, input.lightDir));
	if(lightIntensity > 0.0f) {
		color += (diffuseColor * lightIntensity);
		color = saturate(color);
	}

	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	color = color * textureColor;
    return color;
}
