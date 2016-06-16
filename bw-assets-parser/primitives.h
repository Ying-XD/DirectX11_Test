#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

class IBinaryFile;
class OBinaryFile;

struct PrimitivesAsset
{
    std::map<std::string, std::vector<uint8_t>> sections;

    bool read(IBinaryFile &);
    bool write(OBinaryFile &);
};

