/*
vertices.cpp
Birthday : 9/29/2014 16:54:28
Author   : If
*/

#include "vertices.h"
#include "vertexformats.h"
#include <assert.h>

struct VertexHeader
{
    char    format[ 64 ];
    int32_t nVertices;
};

VertexTypeInfo const VerticesAsset::typeInfo() const
{
    VertexTypeInfo info;
    if(format == "xyznuv") {
        info = {
            "xyznuv", VertexType::XYZNUV, sizeof(VertexXYZNUV)
        };
    } else if(format == "xyznuvtb") {
        info = {
            "xyznuvtb", VertexType::XYZNUVTB, sizeof(VertexXYZNUVTB)
        };
    } else if(format == "xyznuviiiww") {
        info = {
            "xyznuviiiww", VertexType::XYZNUVIIIWW, sizeof(VertexXYZNUVIIIWW)
        };
    } else if(format == "xyznuviiiwwtb") {
        info = {
            "xyznuviiiwwtb", VertexType::XYZNUVIIIWWTB, sizeof(VertexXYZNUVIIIWWTB)
        };
    } else if(format == "xyznuviiiiwww") {
        info = {
            "xyznuviiiiwww", VertexType::XYZNUVIIIIWWW, sizeof(VertexXYZNUVIIIIWWW)
        };
    } else if(format == "xyznuviiiiwwwtb") {
        info = {
            "xyznuviiiiwwwtb", VertexType::XYZNUVIIIIWWWTB, sizeof(VertexXYZNUVIIIIWWWTB)
        };
    } else {
        info = {
            "unknown", VertexType::UNKNOWN, size_t(-1)
        };
    }

    return info;
}

VerticesAsset::VerticesAsset(std::vector<uint8_t> const & data)
{
    uint8_t const* cursor = static_cast<uint8_t const*>(data.data());
    VertexHeader header = *reinterpret_cast<VertexHeader const*>(cursor);
    cursor += sizeof(VertexHeader);
    this->format = header.format;
    VertexTypeInfo const info = typeInfo();
    assert(info.stride > 0 && sizeof(VertexHeader) + header.nVertices*info.stride <= data.size());
    this->data.assign(cursor, cursor+header.nVertices*info.stride );
}


