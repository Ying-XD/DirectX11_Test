//----------------------------------
//  FullScreenProcess.hlsl
//----------------------------------
Texture2D screenTex             : register(t0);
SamplerState SampleTypeClamp    : register(s0);
cbuffer global {
    float    sm_width;
    float3   padding;
};

struct FullScreenVSOut {
    float4 pos          : SV_POSITION;
    float2 uv           : TEXTURE0;
};

FullScreenVSOut VS_FullScreenProcess(uint VertexID : SV_VertexID) {
    FullScreenVSOut output;
    output.uv = float2((VertexID << 1) & 2, VertexID & 2);
    output.pos = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    //output.depthPos = output.pos;
    return output;
}
float4 PS_FullScreenRGBA(FullScreenVSOut input) : SV_TARGET{
    float4 color = screenTex.Sample(SampleTypeClamp,input.uv);
    return color;
}

float4 PS_FullScreenProcess(FullScreenVSOut input) : SV_TARGET{
    float r;
    r = screenTex.Sample(SampleTypeClamp,input.uv).r;
    if (r >= 1.05f)
        r = 0.0f;
    return float4(r,r,r,1);
}

float4 PS_WarpMaps2(FullScreenVSOut input) {
    float r;

    float x = input.uv.x * sm_width;
    uint xf = floor(x);
    uint xc = ceil(x);
    float r1,r2;
    float color;


    r1 = screenTex[uint2(xf,0)].r;
    r2 = screenTex[uint2(xc,0)].r;

    if (r2 - r1 <=1e-5)
        r = r1;
    else
        r = lerp(r1,r2,(x-xf)/(xc-xf));
    color = r;

    float y = input.uv.y * sm_width;
    uint yf = floor(y);
    uint yc = ceil(y);

    r1 = screenTex[uint2(yf,1)].r;
    r2 = screenTex[uint2(yc,1)].r;

    if (r2 - r1 <=1e-5)
        r = r1;
    else
        r = lerp(r1,r2,(x-xf)/(xc-xf));
    color += r;
    color /= 2;
    return float4(color, color, color, 1.0f);
}

float4 PS_WarpMaps(FullScreenVSOut input) : SV_TARGET{
    return PS_WarpMaps2(input);
    float r;

    float x = input.uv.x * sm_width;
    uint xf = floor(x);
    uint xc = ceil(x);
    uint y;
    float r1,r2;
    
    //if (depthPos.y <= 0.0f)
    if (input.uv.y < 0.5f)
        y = 1;
    else
        y = 0;
    r1 = screenTex[uint2(xf,y)].r;
    r2 = screenTex[uint2(xc,y)].r;

    if (r2 - r1 <=1e-5)
        r = r1;
    else
        r = lerp(r1,r2,(x-xf)/(xc-xf));
    
    if (r <= 0.0f || r >= 1.0f) 
       return float4(1.0f, 0, 0, 1);
    return float4(r,r,r,1);
}

