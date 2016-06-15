//----------------------------------
//  importancePS.hlsl
//----------------------------------

//  --- TEXTURES    ---
Texture2D depthLightTex : register(t0);
Texture2D nvTex         : register(t1);
Texture2D depthViewTex  : register(t2);




struct PixelInputType {
    float4  pos         : SV_POSITION;
    float2  uv          : TEXTURE0;
    float4  depthPos    : COLOR0;
};

cbuffer ConstBuffer {
    matrix  lv2cv;
    uint    sm_wdith;
    float   sn_bonus;
    float   shadowBias;
    float   padding;
}


// shadow edge func
float ImpShadowEdge(float d[4]) {
    if (d[0] >= 1.0f)
        return 0.001f;
    bool dd = false;
    for (int i = 1; i < 4; i++) {
        dd = dd || (d[i] < 1.0f && abs(d[0] - d[i]) > shadowBias);
    }
    return dd ? 1.0f : 0.8f;
}

float ImpSurfaceNormal(float n[4]) {
    float maxN = max(max(n[0], n[1]), max(n[2], n[3]));
    return (1.0f + sn_bonus * clamp(maxN, 0, 1));
}

float ImpDistance(float2 p[4], float d[4]) {
    if (d[0] >= 1.0f)
        return -1;

    float4 pnt;
    float minZ = 2.0f;
    bool flag = true;

    for (int i = 0; i < 4; i++) {
        pnt = mul(float4(p[i], d[i], 1.0f), lv2cv);
        pnt.xyz = pnt.xyz / pnt.w;
        if (abs(pnt.x) <= 1.05f && abs(pnt.y <= 1.05f) && pnt.w > 0) {
            minZ = min(minZ, pnt.z);
            flag = false;
        }
    }
    if (true && flag) {
        pnt = mul(float4(p[0], d[0], 1.0f), lv2cv);
        float f = max(abs(pnt.x), abs(pnt.y));
        return (10 - f) * 0.01;
    }

    return clamp(1.0f - minZ, 0.0001f, 1.0f);

}

float ImportancePS_Forward(PixelInputType input) : SV_TARGET {
    

    float   imp = 1.0f;
    float2  pos[4];
    float   depth[4];
    float   nv[4];
    float   dd_texSize = 2.0f / sm_wdith;


    float2 xy = input.depthPos.xy / input.depthPos.w;
    pos[0] = xy + float2(0.0f, 0.0f);
    pos[1] = xy + float2(dd_texSize, 0.0f);
    pos[2] = xy + float2(0.0f, dd_texSize);
    pos[3] = xy + float2(dd_texSize, dd_texSize);


    uint2 uv0 = uint2(sm_wdith * input.uv.x, sm_wdith *input.uv.y);
    uint2 uv[4];
    uv[0] = uv0 + uint2(0, 0);
    uv[1] = uv0 + uint2(1, 0);
    uv[2] = uv0 + uint2(0, 1);
    uv[3] = uv0 + uint2(1, 1);
    for (int i = 0; i < 4; i++) {
        uv[i].x = clamp(uv[i].x, 0, sm_wdith-1);
        uv[i].y = clamp(uv[i].y, 0, sm_wdith-1);

        depth[i] = depthLightTex[uv[i]].r;
        nv[i] = nvTex[uv[i]].r;
    }
    
    imp *= ImpDistance(pos, depth);
    imp *= ImpSurfaceNormal(nv);
    imp *= ImpShadowEdge(depth);
    
    return imp;

}

