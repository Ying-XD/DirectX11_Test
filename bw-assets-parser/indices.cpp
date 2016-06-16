/*
indices.cpp
Birthday : 9/28/2014 17:53:59
Author   : If
*/

#include "indices.h"
#include "string.h"
#include <assert.h>

IndicesAsset::IndicesAsset(std::vector<uint8_t> const& content)
{
    struct IndexHeader {
        char    format[ 64 ];
        int32_t nIndices;
        int32_t nGroups;
    };
    uint8_t const* data = static_cast<uint8_t const*>(content.data());
    uint8_t const* cursor = data;
    size_t         size = content.size();

    IndexHeader header = *reinterpret_cast<IndexHeader const*>(cursor);
    cursor += sizeof(IndexHeader);

    if(!strncmp("list32", header.format, 7)) {
        format = "list32";
        assert(cursor + sizeof(uint32_t)*header.nIndices + sizeof(IndexGroup)*header.nGroups <= data+size);
        index32.assign(reinterpret_cast<uint32_t const*>(cursor), reinterpret_cast<uint32_t const*>(cursor + sizeof(uint32_t)*header.nIndices));
        cursor += sizeof(uint32_t)*header.nIndices;
    } else if(!strncmp("list", header.format, 5)) {
        format = "list";
        assert(cursor + sizeof(uint16_t)*header.nIndices + sizeof(IndexGroup)*header.nGroups <= data+size);
        index16.assign(reinterpret_cast<uint16_t const*>(cursor), reinterpret_cast<uint16_t const*>(cursor + sizeof(uint16_t)*header.nIndices));
        cursor += sizeof(uint16_t)*header.nIndices;
    } else {
        // unknown format;
        assert(false && "unknown index format");
        return;
    }

    for(int32_t i=0; i<header.nGroups; ++i) {
        groups.push_back(*reinterpret_cast<IndexGroup const*>(cursor));
        cursor += sizeof(IndexGroup);
    }
}

