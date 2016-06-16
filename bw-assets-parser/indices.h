#pragma once

#include <stdint.h>
#include <vector>
#include <string>

struct IndicesAsset
{
    std::vector<uint16_t>   index16;
    std::vector<uint32_t>   index32;
    std::string             format;

    struct IndexGroup {
        int32_t startIndex, numIndex, startVertex, numVertex;
    };
    std::vector<IndexGroup> groups;

    IndicesAsset(std::vector<uint8_t> const& content);
};
