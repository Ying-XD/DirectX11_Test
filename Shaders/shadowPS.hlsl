////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.ps
////////////////////////////////////////////////////////////////////////////////

Texture2D shaderTexture     : register(t0);
Texture2D shadowTexture     : register(t1);
Texture2D warpMaps          : register(t2);

SamplerState SampleTypeWrap  : register(s0);
SamplerState SampleTypeClamp : register(s1);
SamplerState SampleTypeBorder : register(s2);

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4  ambientColor;
	float4  diffuseColor;
    uint    rtwFlag;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
	float3 lightDir : TEXCOORD2;
};


float2 GetWarpUV(float2 uv) {
    if (uv.x > 1.0f) uv.x = 1.025f;
    else if (uv.x < 0.0f) uv.x = -0.025f;
    else    uv.x += warpMaps.Sample(SampleTypeWrap, float2(uv.x, .25f)).r;

    if (uv.y > 1.0f) uv.y =   1.025f;
    else if (uv.y < 0.0f) uv.y = -0.025f;
    else    uv.y += warpMaps.Sample(SampleTypeWrap, float2(uv.y, .75f)).r;

    return uv;
}
float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
	float bias;
    float4 color;
	float2 rtwCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
	float4 textureColor;
	// Set the bias value for fixing the floating point precision issues.
	bias = 0.0001f;
   
	// 环境光
    color = ambientColor;
    
    rtwCoord.xy = input.lightViewPosition.xy / input.lightViewPosition.w * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    if (rtwFlag != 0) rtwCoord.xy = GetWarpUV(rtwCoord.xy);

    if ((saturate(rtwCoord.x) != rtwCoord.x) || (saturate(rtwCoord.y) != rtwCoord.y)) {
            lightIntensity = saturate(dot(input.normal, input.lightDir));
		    if(lightIntensity > 0.0f) {
				color += (diffuseColor * lightIntensity);
				color = saturate(color);
			}
        //color = float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        // 采样获得lightView下的 深度纹理 中 该 采样点的值。
		depthValue = shadowTexture.Sample(SampleTypeClamp, rtwCoord).r;
        // 计算当前点在lightView下的深度
        lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

        // 因为浮点数存在误差，需要一个 补偿值 来判断 lightDepthValue<= depthValue情况。
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if(lightDepthValue <= depthValue)
		{
		    // Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(input.normal, input.lightDir));

		    if(lightIntensity > 0.0f)
			{
				// 加上漫反射的光
				color += (diffuseColor * lightIntensity);
				color = saturate(color);
                if (rtwFlag == 4)
                    color = float4(1.0f, 0.0f, 0.0f, 1.0f);
			}
            else{
            if (rtwFlag == 4)
                color = float4(0.0f, 0.0f, 0.0f, 1.0f);
                }
		}
        else{
            if (rtwFlag == 4)
                color = float4(1.0f, 1.0f, 0.0f, 1.0f);
            }
        if (rtwFlag == 2)return float4(depthValue,depthValue,depthValue,1.0f);
        if (rtwFlag == 3)return lightDepthValue;

    }
    
    // 对纹理进行采样，计算出像素的颜色
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// 结合光照和像素颜色
	color = color * textureColor;
    return color;
}
