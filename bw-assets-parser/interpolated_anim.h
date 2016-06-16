#pragma once

#include <string>
#include <vector>
#include <utility>
#include "vertexformats.h"

class IBinaryFile;
class OBinaryFile;

struct InterpolatedChannel
{
    int type;
    std::string channel;

    bool  processCompressInfo;
    bool  processVisibilityInfo;
    float scaleCompressionError,
          positionCompressionError,
          rotationCompressionError;

    typedef std::vector< std::pair< float, Vector3 > > ScaleKeyframes;
    typedef std::vector< std::pair< float, Vector3 > > PositionKeyframes;
    typedef std::vector< std::pair< float, Vector4 > > RotationKeyframes;
    typedef std::vector< std::pair< float, float   > > VisibilityKeyframes;

    ScaleKeyframes      scaleKeys      ;
    PositionKeyframes   positionKeys   ;
    RotationKeyframes   rotationKeys   ;
    VisibilityKeyframes visibilityKeys ;
};

struct InterpolatedAnimation
{
    float totalTime;
    std::string identifier, internalIdentifier;
    int   nChannels;

    std::vector<std::pair<std::string, InterpolatedChannel> > channels;

    InterpolatedAnimation():totalTime(0), identifier(""), internalIdentifier(""), nChannels(0) {}
    InterpolatedAnimation(std::string const& fn);
    bool read(IBinaryFile &);
    bool write(OBinaryFile &);
};

