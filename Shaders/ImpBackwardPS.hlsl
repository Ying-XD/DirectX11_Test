
struct ImpPSBackwardIn {
    float4  pos : SV_Position;
    float   dpt : DEPTH;
    float   nv  : NVTEXTURE;
};


float ImpDistance(float dpt) {
    return clamp(1.0f - dpt, 0.0001f, 1.0f) * 5.0f;
}
float ImpSurfaceNormal(float nv) {
    float sn_bonus = 2.0f;
    return (1.0f + sn_bonus * clamp(nv, 0, 1));
}
float ImportancePS_Backward(ImpPSBackwardIn input) : SV_TARGET {
    
/*
    float imp = 1.0f;
    imp *= ImpDistance(dpt);
    imp *= ImpSurfaceNormal(nv);
//*/
    float imp = 1.0f;
    imp *= ImpDistance(input.dpt);
    //imp *= ImpSurfaceNormal(input.nv);
    return  imp;

    if (input.dpt > 1.0f ) return 0.0f;
    return 1.0f;
   
    
}