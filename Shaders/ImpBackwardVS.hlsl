//----------------------------------
//  importanceVS.hlsl
//----------------------------------
//  --- TEXTURES    ---
Texture2D depthTex      : register(t0);
Texture2D nvTex         : register(t1);

SamplerState SamplerWrap            : register(s0);

cbuffer glValue{
    matrix  cv2lv;
    uint    sm_width;
    float3  padding;
};
struct ImpVSBackwardOut {
    float4  pos : SV_Position;
    float   dpt : DEPTH;
    float   nv  : NVTEXTURE;
};

ImpVSBackwardOut ImportanceVS_Backward(uint VertexID : SV_VertexID) {
    ImpVSBackwardOut output;
    float sm_d = 1.0f / sm_width;
    
    uint2 uv = uint2(VertexID % sm_width, VertexID / sm_width);
    float2 f_uv = float2(uv * sm_d);

    float dpt = depthTex[uv].r;
    float4 c_pos = float4(f_uv.x * 2.0f - 1.0f, 1.0f - f_uv.y * 2.0f, dpt, 1.0f);

    output.pos  = mul(c_pos, cv2lv);
    output.dpt  = dpt;
    output.nv   = nvTex[uv].r;

    //float2 uv = float2((VertexID << 1) & 2, VertexID & 2);
    //output.pos = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    //output.uv = uv * sm_width;

    return output;
}
