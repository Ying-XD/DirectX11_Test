Texture2D       warpMaps    : register(t0);
Texture2D       colorTex    : register(t1);
Texture2D       lightDepTex : register(t2);
Texture2D       viewDepTex  : register(t3);
Texture2D       diffuseTex  : register(t4);

SamplerState SampleTypeWrap     : register(s0);
SamplerState SampleTypeClamp    : register(s1);

cbuffer PSCbuffer {
    matrix  cv_2_lv;
    float4  ambientColor;
    float2  shadow_texel_size;
    float2  padding;
};

struct PS_INPUT {
    float4 pos          : SV_POSITION;
    float2 uv           : TEXTURE0;
};


float2 GetWarp(float2 uv) {
    uv.x = warpMaps.Sample(SampleTypeClamp, float2(uv.x, 0.25f)).r;
    uv.y = warpMaps.Sample(SampleTypeClamp, float2(uv.y, 0.75f)).r;
    return uv;
}

float ShadowMap(float2 loc, float pixel_z) {
    if (saturate(loc.x) != loc.x || saturate(loc.y) != loc.y)
        return 1.0f;

    float lightDep = lightDepTex.Sample(SampleTypeWrap, loc).r;
    float shadow_bias = 0.0005f;
    float z_diff = pixel_z - lightDep;
    if (pixel_z - shadow_bias <= lightDep)
        return 1.0f;
    return 0.0f;

    return 1 - clamp( (z_diff - shadow_bias) / shadow_bias, 0, 1);
}

float Gaussian(float2 x, float a, float2 b, float c) {
    return  a * exp(dot(x - b, x - b) / (2*c*c));
}

float ShadowMapPCF_Soft(float2 center, float pixel_z, uint pcf_size){
    float   jitter[9] = { 0.001, -0.002, 0.0015, -0.0005, -0.0002, 0.0001, 0.00015, 0.00005, 0.000025 };
    float2  shadow_texel_size = float2(1.0 / 512.0, 1.0 / 512.0);
    float2  base_loc    = center - shadow_texel_size.xy * (pcf_size-1) / 2.0f;
    float2  c_loc       = base_loc;
    float   intensity   = 0.0f;
    float   weight      = 0.0f;
    uint    c_jit       = 0;
    float   cwgt        = 0.0f;
    float2  m_jit, t_loc, t_uv;


    uint i = 0,j = 0;
    for (uint kk = 0; kk < pcf_size * pcf_size; kk++) {
        t_loc.x = c_loc.x + jitter[c_jit];
        t_loc.y = c_loc.y + jitter[c_jit + 1];

        c_jit   = (c_jit + 2) & 7;
        cwgt    = Gaussian(t_loc, 1, center,1);
        intensity += ShadowMap(GetWarp(t_loc), pixel_z) * cwgt;
        weight  += cwgt;

        c_loc.x += shadow_texel_size.x;
        j += 1;

        if (j == pcf_size) {
            j = 0;
            i += 1;
            c_loc.x = base_loc.x;
            c_loc.y += shadow_texel_size.y;
        }
    }
    return intensity / weight;
/*
    float2 t_loc = GetWarp(c_loc);
    intensity = ShadowMap(t_loc,pixel_z);

    return intensity;
//*/
}


PS_INPUT Backward_VS(uint VertexID : SV_VertexID) {
    PS_INPUT output;
    output.uv = float2((VertexID << 1) & 2, VertexID & 2);
    output.pos = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}
float4 Backward_PS(PS_INPUT input): SV_TARGET {

    float4  color = colorTex.Sample(SampleTypeWrap, input.uv);
    float   dpt = viewDepTex.Sample(SampleTypeWrap, input.uv).r;
    float4  diffuseColor = diffuseTex.Sample(SampleTypeWrap, input.uv);
    float   intensity = 1.0f;
    
    if (dpt < 1.0f) {
        float4 vpos = float4(input.uv.x * 2.0f - 1.0f, input.uv.y * -2.0f + 1.0f, dpt, 1.0f);
        float4 lpos = mul(vpos, cv_2_lv);
        
        float2 shadow_uv = lpos.xy / lpos.w * float2(.5f, -.5f) + float2(.5f, .5f);
        float  lightDep  = lpos.z / lpos.w;

        //intensity = ShadowMapPCF_Soft(shadow_uv, lightDep, 3);
        intensity = ShadowMap(GetWarp(shadow_uv), lightDep);
    }

    if (diffuseColor.x > 0.0f && intensity > 0.0f) {
        color *= saturate(diffuseColor + ambientColor);
    }
    else {
        color *= ambientColor;
    }
    //color = color * lerp(0.2f, 1.0f, intensity);
    return color;
}