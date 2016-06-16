/*
primities.cpp
Birthday : 9/28/2014 16:20:49
Author   : If
*/

#include "primitives.h"
#include "binaryfile.h"
#include "array.h"


bool PrimitivesAsset::read(IBinaryFile& file)
{
    uint32_t magic;
    file.read(magic);
    if(magic!=0x42a14e65)
        return false;
    file.seekg(-4, std::ios::end);
    std::ios::pos_type eofpos = file.tellg();
    int32_t indexSize = 0;
    int32_t offset = 0;
    file.read(indexSize);
    int32_t contentSize = int32_t(eofpos)-indexSize-4;

    file.seekg(4);
    Array<uint8_t> arr(contentSize);
    file.read<uint8_t>(static_cast<uint8_t*>(arr.data()), arr.size());

    sections.clear();
    file.seekg(std::streamoff(-4-indexSize), std::ios::end);
    while(file.tellg() < eofpos) {
        uint32_t entryDataLen = 0;
        std::string entryName;
        file.read(entryDataLen);
        file.seekg(16, std::ios::cur);
        file.read(entryName);

        std::ios::pos_type cur = file.tellg();
        if(((uint32_t(cur)+3)&(~3ul)) != cur)
            file.seekg(std::streampos((uint32_t(cur)+3)&(~3ul)));

        Array<uint8_t> section = arr.slice(offset, entryDataLen);
        sections[entryName].assign(section.begin(), section.end());

        offset += entryDataLen;
        offset += 3;
        offset &= ~3ul;
    }

    return file.good();
}

bool PrimitivesAsset::write(OBinaryFile& file)
{
    struct PrimitivesItemIndex {
        int32_t offset, length;
    };

    uint32_t const magic=0x42a14e65;
    file.write(magic);
    uint32_t offset=4;
    uint8_t const padding[16] = {0};
    for(auto const& section:sections) {
        file.write<uint8_t>(static_cast<uint8_t const*>(section.second.data()), section.second.size());
        offset += section.second.size();
        uint32_t newoffset = offset + 3;
        newoffset &= ~3ul;
        file.write<uint8_t>(padding, newoffset-offset);
        offset = newoffset;
    }
    offset = 0;
    for(auto const& section:sections) {
        file.write<uint32_t>(section.second.size());
        file.write<uint8_t>(padding, 16);
        file.write(section.first);
        offset += 4 + 16 + 4 + section.first.size();

        uint32_t newoffset = offset + 3;
        newoffset &= ~3ul;
        file.write<uint8_t>(padding, newoffset-offset);
        offset = newoffset;
    }
    file.write(offset);

    return false;
}

