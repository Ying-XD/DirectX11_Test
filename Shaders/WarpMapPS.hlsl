Texture2D impTex    : register(t0);
Texture2D compactTex: register(t1);
Texture2D blurTex   : register(t2);

SamplerState SampleTypeWrap     : register(s0);

cbuffer cosntBuffer {
    float   d_texSize;
    uint    sm_width;
    float   blur_factor;
    uint    blur_width;   
};

struct WarpMapPSIn {
    float4 pos          : SV_POSITION;
    float2 uv           : TEXTURE0;
};


float rtw_compact(WarpMapPSIn input) : SV_TARGET {
    
    uint u = (uint)(input.uv.x * sm_width);

    float wgt = -1;
    if (input.uv.y < 0.5){          // XÖá
        for (uint v = 0;v < sm_width; v++){
            float cw = impTex[uint2(u,v)].r;
            wgt = max(wgt,cw);
        }
    }
    else {                          // YÖá
        for (uint v = 0;v < sm_width; v++){
            float cw = impTex[uint2(v,u)].r;
            wgt = max(wgt,cw);
        }
    }

	return wgt;

}

float blur_getValue(int2 uv) {
    if (uv.x < 0 || uv.x >= sm_width) return 0;
    if (uv.x < 0) 
        uv.x = 0;
    if (uv.x >= sm_width) 
        uv.x = sm_width -1;
    
    return compactTex[uv].r;
}
float rtw_blur(WarpMapPSIn input) : SV_TARGET {

    uint2   uv = uint2(input.uv * uint2(sm_width, 2));
      
    float   my_w = compactTex[uv].r;
    float   c_factor = blur_factor;

    for (uint du = 1; du < blur_width; du++) {
        float w0 = blur_getValue(uv - uint2(du, 0));
        float w1 = blur_getValue(uv + uint2(du, 0));
        float cw = max(w0, w1) * c_factor;
        if (cw >= 0) {
            my_w = max(my_w, cw);
        }
        c_factor *= blur_factor;
    }

    return my_w;    

 
}
int2 GetBoundingRegion(uint v) {
    int first_u = sm_width, last_u = 0;
    float cw;
    for (uint u = 0; u < sm_width; u++) {
        cw = blurTex[uint2(u,v)].r;
        if (cw > 0){
            first_u = u;
            break;
        }
    }
    for (int iu = sm_width - 1; iu > first_u; iu--) {
        cw = blurTex[uint2(iu,v)].r;
        if (cw > 0) {
            last_u = iu;
            break;
        }
    }
    if (last_u < first_u) return int2(0, sm_width);
    return int2(first_u, last_u + 1);
}


float GetWeight(uint iu0 ,uint iu1, uint v) {
 
    float ret = 0;
    float  cw;
    for (uint iu = iu0;iu < iu1; ++iu){
        cw = blurTex[uint2(iu,v)].r;
        ret += max(cw, 0.001f);
    }
    return ret;
}

float rtw_build(WarpMapPSIn input) : SV_TARGET {
    uint my_u = (uint)(input.uv.x * sm_width);
    uint my_v = (uint)(input.uv.y * 2);
    int2 range = GetBoundingRegion(my_v);


    float st_loc = input.uv.x;
    float  my_loc = st_loc;

    if (my_u < range.x)
        my_loc = -0.025f;
    else if (my_u > range.y) {
        my_loc = 1.025f;
    } else {
        float w0 = GetWeight(range.x, my_u, my_v);
        float w1 = GetWeight(my_u, range.y, my_v);

        my_loc = lerp(0.0f, 1.0f, w0 / (w0+w1));
    }
    //return (my_loc);
    return (my_loc - st_loc);

}