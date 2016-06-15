Texture2D warpMaps              : register(t0);

SamplerState SampleTypeBorder   : register(s0);

//  ----------  Tessellation.hlsl   ----------
cbuffer TessellationBuffer : register(b0){
    matrix  worldMatr;
    matrix  viewMatr;
    matrix  projMatr;
    float   tessAmount;
    float3  padding;
};


// -----    struct  -----
struct VS_INPUT {
    float3  pos         : POSITION;
    float2  uv          : TEXCOORD0;
    float3  normal      : NORMAL;
};
struct HS_INPUT {
    float3  pos         : POSITION;
};
struct PatchTess{
    float Edges[3]      : SV_TessFactor;
    float Inside        : SV_InsideTessFactor;
};

struct DS_INPUT {
    float3 pos          : POSITION;
};

struct PS_INPUT {
    float4 pos          : SV_POSITION;
    float4 dpt          : POSITION;
};

// ----------   Vertex Shader   ----------
HS_INPUT TessellationVS(VS_INPUT input) {
    HS_INPUT output;
    output.pos = input.pos;
    return output;
}

// ----------   Hull Shader     ----------

PatchTess ConstantHS(InputPatch<HS_INPUT, 3> inputPatch, 
        uint patchId : SV_PrimitiveID) {

    PatchTess output;
    output.Edges[0] = tessAmount;
    output.Edges[1] = tessAmount;
    output.Edges[2] = tessAmount;
    output.Inside   = tessAmount;

    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]

DS_INPUT TessellationHS(InputPatch<HS_INPUT, 3> patch,
        uint pointId : SV_OutputControlPointID,
        uint patchId : SV_PrimitiveID
) {
    DS_INPUT output;
    output.pos = patch[pointId].pos;
    return output;
}

// ----------   Domain Shader   ----------
float2 GetWarpPos(float2 uv) {
    if (uv.x >= 1.0f) uv.x = 1.025f;
    else if(uv.x < -0.0f) uv.x = -0.25f;
    else uv.x += warpMaps.SampleLevel(SampleTypeBorder, float2(uv.x, .25f), 0).r;

    if (uv.y >= 1.0f) uv.y = -0.025f;
    else if(uv.y < 0.0f) uv.y = 1.025f;
    else uv.y += warpMaps.SampleLevel(SampleTypeBorder, float2(uv.y, .75f), 0).r;

    return uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
}
[domain("tri")]
PS_INPUT TessellationDS(PatchTess input, 
    float3 uvwCoord : SV_DomainLocation, 
    const OutputPatch<DS_INPUT, 3> patch) 
{
    
    PS_INPUT output;

    output.pos.xyz = uvwCoord.x * patch[0].pos + uvwCoord.y* patch[1].pos + uvwCoord.z * patch[2].pos;
    output.pos.w = 1.0f;

    output.pos = mul(output.pos, worldMatr);
    output.pos = mul(output.pos, viewMatr);
    output.pos = mul(output.pos, projMatr);
    output.dpt = output.pos;

    float x = output.pos.x / output.pos.w * 0.5 + 0.5;
    float y = -output.pos.y / output.pos.w * 0.5 + 0.5;
    output.pos.xy = GetWarpPos(float2(x,y)) * output.pos.w;
    return output;
}

//  ----------  Pixel Shader    ----------
float TessellationPS(PS_INPUT input) : SV_TARGET {
    float depth = input.dpt.z / input.dpt.w;
    return depth;
}
