#pragma once

#include <string>
#include <vector>
#include <stdint.h>

class IBinaryFile;

struct VertexType {
    enum Enum {
        XYZNUV,
        XYZNUVTB,
        XYZNUVIIIWW,
        XYZNUVIIIIWWW,
        XYZNUVIIIWWTB,
        XYZNUVIIIIWWWTB,
        UNKNOWN
    };
};

struct VertexTypeInfo {
    char const*       name;
    VertexType::Enum  type;
    size_t            stride;
};

struct VerticesAsset
{
    std::string             format;
    std::vector<uint8_t>    data;
    VertexTypeInfo const    typeInfo() const;

    VerticesAsset(std::vector<uint8_t> const &);
};


