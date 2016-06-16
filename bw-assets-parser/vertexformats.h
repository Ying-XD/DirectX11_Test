#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#pragma once

#include "math3d.h"
#include <stdint.h>

typedef vec2 Vector2;
typedef vec3 Vector3;
typedef vec4 Vector4;

// structures {{{
#pragma pack(push, 1 )

/* Position, Normal, UV */
struct VertexXYZNUV {
    Vector3        pos;
    Vector3        normal;
    Vector2        uv;
};

/* Position, Normal, UV, UV2 */
struct VertexXYZNUV2 {
    Vector3        pos;
    Vector3        normal;
    Vector2        uv;
    Vector2        uv2;
};

/* Position, Normal, Colour, UV */
struct VertexXYZNDUV {
    Vector3        pos;
    Vector3        normal;
    uint32_t       colour;
    Vector2        uv;
};

/* Position, Normal */
struct VertexXYZN {
    Vector3        pos;
    Vector3        normal;
};

/* Position, Normal, Colour */
struct VertexXYZND {
    Vector3      pos;
    Vector3      normal;
    uint32_t     colour;
};

/* Position, Colour */
struct VertexXYZL {
    Vector3      pos;
    uint32_t     colour;
};

/* Position only */
struct VertexXYZ {
    Vector3     pos;
};

/* Position, UV */
struct VertexXYZUV {
    Vector3        pos;
    Vector2        uv;
};

/* Position, UV, UV2 */
struct VertexXYZUV2 {
    Vector3        pos;
    Vector2        uv;
    Vector2        uv2;
};

/* Position, UV, UV2, UV3, UV4 */
struct VertexUV4 {
    Vector4      pos;
    Vector2      uv[4];
};

/* Position, Colour, UV */
struct VertexXYZDUV {
    Vector3      pos;
    uint32_t     colour;
    Vector2      uv;
};

/* Position, Colour, UV, UV2 */
struct VertexXYZDUV2 {
    Vector3      pos;
    uint32_t     colour;
    Vector2      uv;
    Vector2      uv2;
};

/* Normal, UV */
struct VertexNUV {
    Vector3      normal;
    Vector2      uv;
};

/* Normal only */
struct VertexN {
    Vector3      normal;
};

/* UV only */
struct VertexUV {
    Vector2      uv;
};

/* Position, Normal, UV, Index */
struct VertexXYZNUVI {
    Vector3      pos;
    Vector3      normal;
    Vector2      uv;
    float        index;
};

/* Y, Normal */
struct VertexYN {
    float        y;
    Vector3      normal;
};

/* Four-component position, Colour */
struct VertexTL {
    Vector4      pos;
    uint32_t     colour;
};

/* Four-component position, Colour, UV */
struct VertexTLUV {
    Vector4      pos;
    uint32_t     colour;
    Vector2      uv;
};

/* Four-component position, Colour, Specular, UV, UV2 */
struct VertexTDSUV2 {
    Vector4      pos;
    uint32_t     colour;
    uint32_t     specular;
    Vector2      uv;
    Vector2      uv2;
};

/* Position, Colour, Specular, UV */
struct VertexXYZDSUV {
    float    x;
    float    y;
    float    z;
    uint32_t colour;
    uint32_t spec;
    float    tu;
    float    tv;
};

/* Position, Colour, Specular, UVUUVV, UVUUVV */
struct VertexTDSUVUUVV2 {
    Vector4      pos;
    uint32_t     colour;
    uint32_t     specular;
    Vector4      uvuuvv;
    Vector4      uvuuvv2;
};

/* Position, Normal, UV, Packed Tangent, Packed Binormal */
struct VertexXYZNUVTB {
    Vector3       pos;
    uint32_t      normal;
    Vector2       uv;
    uint32_t      tangent;
    uint32_t      binormal;
};

/* Unpack compressed normal into a three float vector.  */
inline Vector3 unpackNormal( uint32_t packed )
{
    int32_t z = int32_t(packed) >> 22;
    int32_t y = int32_t( packed << 10 ) >> 21;
    int32_t x = int32_t( packed << 21 ) >> 21;
    return Vector3(float( x ) / 1023.f, float( y ) / 1023.f, float( z ) / 511.f);
}

/* Pack three float normal (each component clamped to [-1,1]) into a single unsigned 32bit word ( 11 bits x, 11 bits y, 10 bits z ) */
inline uint32_t packNormal( const Vector3& nn )
{
    Vector3 n = nn;
    float l = sqrt( n.x*n.x + n.y*n.y + n.z*n.z );
    n.x/=l; n.y/=l; n.z/=l;
    n.x = clamp(-1.f, n.x, 1.f);
    n.y = clamp(-1.f, n.y, 1.f);
    n.z = clamp(-1.f, n.z, 1.f);
    return    ( ( ( (uint32_t)(n.z * 511.0f) )  & 0x3ff ) << 22L ) |
              ( ( ( (uint32_t)(n.y * 1023.0f) ) & 0x7ff ) << 11L ) |
              ( ( ( (uint32_t)(n.x * 1023.0f) ) & 0x7ff ) <<  0L );
}

/* Position, Normal, UV, Tangent, Binormal */
struct VertexXYZNUVTBPC {
    Vector3       pos;
    Vector3       normal;
    Vector2       uv;
    Vector3       tangent;
    Vector3       binormal;
    VertexXYZNUVTBPC& operator =(const VertexXYZNUVTB& in) {
        pos = in.pos;
        normal = unpackNormal( in.normal );
        uv = in.uv;
        tangent = unpackNormal( in.tangent );
        binormal = unpackNormal( in.binormal );
        return *this;
    }
    VertexXYZNUVTBPC(const VertexXYZNUVTB& in) {
        *this = in;
    }
    VertexXYZNUVTBPC& operator =(const VertexXYZNUV& in) {
        pos = in.pos;
        normal = in.normal;
        uv = in.uv;
        tangent = vec3(0,0,0);
        binormal = vec3(0,0,0);
        return *this;
    }
    VertexXYZNUVTBPC(const VertexXYZNUV& in) {
        *this = in;
    }
    VertexXYZNUVTBPC() {
    }
};

/* Position, Normal, UVx2, Packed Tangent, Packed Binormal */
struct VertexXYZNUV2TB {
    Vector3        pos;
    uint32_t       normal;
    Vector2        uv;
    Vector2        uv2;
    uint32_t       tangent;
    uint32_t       binormal;
};


/* Position, Normal, UVx2, Tangent, Binormal */
struct VertexXYZNUV2TBPC {
    Vector3        pos;
    Vector3        normal;
    Vector2        uv;
    Vector2        uv2;
    Vector3        tangent;
    Vector3        binormal;
    VertexXYZNUV2TBPC& operator =(const VertexXYZNUV2TB& in) {
        pos = in.pos;
        normal = unpackNormal( in.normal );
        uv = in.uv;
        uv2 = in.uv2;
        tangent = unpackNormal( in.tangent );
        binormal = unpackNormal( in.binormal );
        return *this;
    }
    VertexXYZNUV2TBPC(const VertexXYZNUV2TB& in) {
        *this = in;
    }
    VertexXYZNUV2TBPC& operator =(const VertexXYZNUV2& in) {
        pos = in.pos;
        normal = in.normal;
        uv = in.uv;
        uv2 = in.uv2;
        tangent = vec3(0,0,0);
        binormal = vec3(0,0,0);
        return *this;
    }
    VertexXYZNUV2TBPC(const VertexXYZNUV2& in) {
        *this = in;
    }
    VertexXYZNUV2TBPC() {
    }
};

/* Position, Normal, UV, Index, Packed Tangent, Packed Binormal */
struct VertexXYZNUVITB {
    Vector3      pos;
    uint32_t     normal;
    Vector2      uv;
    float        index;
    uint32_t     tangent;
    uint32_t     binormal;
};

/* Position, Normal, UV, Index, Tangent, Binormal */
struct VertexXYZNUVITBPC {
    Vector3      pos;
    Vector3      normal;
    Vector2      uv;
    float        index;
    Vector3      tangent;
    Vector3      binormal;
    VertexXYZNUVITBPC& operator =(const VertexXYZNUVITB& in) {
        pos = in.pos;
        normal = unpackNormal( in.normal );
        uv = in.uv;
        index = in.index;
        tangent = unpackNormal( in.tangent );
        binormal = unpackNormal( in.binormal );
        return *this;
    }
    VertexXYZNUVITBPC(const VertexXYZNUVITB& in) {
        *this = in;
    }
    VertexXYZNUVITBPC() {
    }
};

/* Position, Normal, UV, index, index2, index3, weight, weight2 */
struct VertexXYZNUVIIIWW {
    Vector3      pos;
    uint32_t     normal;
    Vector2      uv;
    uint8_t      index;
    uint8_t      index2;
    uint8_t      index3;
    uint8_t      weight;
    uint8_t      weight2;
    VertexXYZNUVIIIWW& operator =(const VertexXYZNUVI& in) {
        pos = in.pos;
        normal = packNormal(in.normal);
        uv = in.uv;
        weight = 255;
        weight2 = 0;
        index = (uint8_t)in.index;
        index2 = index;
        index3 = index;
        return *this;
    }
};

/* Position, Normal, UV, index, index2, index3, weight, weight2, packed tangent, packed binormal. */
struct VertexXYZNUVIIIWWTB {
    Vector3      pos;
    uint32_t     normal;
    Vector2      uv;
    uint8_t      index;
    uint8_t      index2;
    uint8_t      index3;
    uint8_t      weight;
    uint8_t      weight2;
    uint32_t     tangent;
    uint32_t     binormal;
    VertexXYZNUVIIIWWTB& operator =(const VertexXYZNUVITB& in) {
        pos = in.pos;
        normal = in.normal;
        uv = in.uv;
        weight = 255;
        weight2 = 0;
        index = (uint8_t)in.index;
        index2 = index;
        index3 = index;
        tangent = in.tangent;
        binormal = in.binormal;
        return *this;
    }
};

struct VertexXYZNUVIIIIWWW {
    Vector3      pos;
    unsigned long      normal;
    Vector2      uv;
    unsigned char      index;
    unsigned char      index2;
    unsigned char      index3;
    unsigned char      index4;
    unsigned char      weight;
    unsigned char      weight2;
    unsigned char      weight3;
};

struct VertexXYZNUVIIIIWWWTB {
    Vector3      pos;
    unsigned long      normal;
    Vector2      uv;
    unsigned char      index;
    unsigned char      index2;
    unsigned char      index3;
    unsigned char      index4;
    unsigned char      weight;
    unsigned char      weight2;
    unsigned char      weight3;
    unsigned long      tangent;
    unsigned long      binormal;
};

/* Position, Normal, UV, index3, index2, index, padding, weight2, weight1, padding.  */
struct VertexXYZNUVIIIWWPC {
    Vector3      pos;
    Vector3      normal;
    Vector2      uv;
    uint8_t      index3;
    uint8_t      index2;
    uint8_t      index;
    uint8_t      pad;
    uint8_t      pad2;
    uint8_t      weight2;
    uint8_t      weight;
    uint8_t      pad3;
    VertexXYZNUVIIIWWPC& operator =(const VertexXYZNUVIIIWW& in) {
        pos = in.pos;
        normal = unpackNormal( in.normal );
        uv = in.uv;
        weight = in.weight;
        weight2 = in.weight2;
        index = in.index;
        index2 = in.index2;
        index3 = in.index3;
        return *this;
    }
    VertexXYZNUVIIIWWPC& operator =(const VertexXYZNUVI& in) {
        pos = in.pos;
        normal = in.normal;
        uv = in.uv;
        weight = 255;
        weight2 = 0;
        index = (uint8_t)in.index;
        index2 = index;
        index3 = index;
        return *this;
    }
    VertexXYZNUVIIIWWPC(const VertexXYZNUVIIIWW& in) {
        *this = in;
    }
    VertexXYZNUVIIIWWPC() {
    }
};

/* Position, Normal, UV, index3, index2, index, padding, weight2, weight, padding, packed tangent, packed binormal.  */
struct VertexXYZNUVIIIWWTBPC {
    Vector3      pos;
    Vector3      normal;
    Vector2      uv;
    uint8_t      index3;
    uint8_t      index2;
    uint8_t      index;
    uint8_t      pad;
    uint8_t      pad2;
    uint8_t      weight2;
    uint8_t      weight;
    uint8_t      pad3;
    Vector3      tangent;
    Vector3      binormal;
    VertexXYZNUVIIIWWTBPC& operator =(const VertexXYZNUVIIIWWTB& in) {
        pos = in.pos;
        normal = unpackNormal( in.normal );
        uv = in.uv;
        weight = in.weight;
        weight2 = in.weight2;
        index = in.index;
        index2 = in.index2;
        index3 = in.index3;
        tangent = unpackNormal( in.tangent );
        binormal = unpackNormal( in.binormal );
        return *this;
    }
    VertexXYZNUVIIIWWTBPC& operator =(const VertexXYZNUVITB& in) {
        pos = in.pos;
        normal = unpackNormal( in.normal );
        uv = in.uv;
        weight = 255;
        weight2 = 0;
        index = (uint8_t)in.index;
        index2 = index;
        index3 = index;
        tangent = unpackNormal( in.tangent );
        binormal = unpackNormal( in.binormal );
        return *this;
    }
    VertexXYZNUVIIIWWTBPC(const VertexXYZNUVIIIWWTB& in) {
        *this = in;
    }
    VertexXYZNUVIIIWWTBPC() {
    }
};

struct VertexXYZNUVIIIIWWWPC
{
    Vector3 pos;
    Vector3 normal;
    Vector2 uv;
    uint8_t index3;
    uint8_t index2;
    uint8_t index;
    uint8_t index4;
    uint8_t weight3;
    uint8_t weight2;
    uint8_t weight;
    uint8_t pad;
    VertexXYZNUVIIIIWWWPC& operator =(const VertexXYZNUVIIIIWWW& in)
    {
        pos = in.pos;
        normal = unpackNormal(in.normal);
        uv = in.uv;
        weight = in.weight;
        weight2 = in.weight2;
        weight3 = in.weight3;
        index = in.index;
        index2 = in.index2;
        index3 = in.index3;
        index4 = in.index4;
        pad = 0;
        return *this;
    }
    VertexXYZNUVIIIIWWWPC(const VertexXYZNUVIIIIWWW& in)
    {
        *this = in;
    }
    VertexXYZNUVIIIIWWWPC()
    {
    }
};

struct VertexXYZNUVIIIIWWWTBPC
{
    Vector3 pos;
    Vector3 normal;
    Vector2 uv;
    uint8_t index3;
    uint8_t index2;
    uint8_t index;
    uint8_t index4;
    uint8_t weight3;
    uint8_t weight2;
    uint8_t weight;
    uint8_t pad;
    Vector4 tangent;
    Vector3 binormal;
    VertexXYZNUVIIIIWWWTBPC& operator =(const VertexXYZNUVIIIIWWWTB& in)
    {
        pos = in.pos;
        normal = unpackNormal(in.normal);
        uv = in.uv;
        weight = in.weight;
        weight2 = in.weight2;
        weight3 = in.weight3;
        index = in.index;
        index2 = in.index2;
        index3 = in.index3;
        index4 = in.index4;
        binormal = unpackNormal(in.binormal);

        tangent = vec4(unpackNormal(in.tangent), 1);
        Vector3 crossV = cross(normal, tangent.xyz());
        float dotV = dot(crossV, binormal);
        if (dotV>0)
            tangent.w = 1;
        else
            tangent.w = -1;
        pad = 0;
        return *this;
    }
    VertexXYZNUVIIIIWWWTBPC(const VertexXYZNUVIIIIWWWTB& in)
    {
        *this = in;
    }
    VertexXYZNUVIIIIWWWTBPC()
    {
    }
};

struct VertexTex7 {
    Vector4        tex1;
    Vector4        tex2;
    Vector4        tex3;
    Vector4        tex4;
    Vector4        tex5;
    Vector4        tex6;
    Vector4        tex7;
    static std::string decl() { return "xyznuv8tb"; }
};

/* Y, Normal, Diffuse, Shadow */
struct VertexYNDS {
    float        y;
    uint32_t     normal;
    uint32_t     diffuse;
    uint16_t     shadow;
};

/* 16 bit U, 16 bit V */
struct VertexUVXB {
    int16_t      u;
    int16_t      v;
};

/* Position, Colour, float size */
struct VertexXYZDP {
    Vector3      pos;
    uint32_t     colour;
    float        size;
};
#pragma pack( pop )
// }}}

#endif

