/*
interpolated_anim.cpp
Birthday : 9/24/2014 23:01:42
Author   : If
*/

#include "binaryfile.h"
#include "interpolated_anim.h"

#include <stdexcept>
#include <stdio.h>

static bool readInterpolatedAnimation(
        IBinaryFile &file,
        InterpolatedChannel& channel,
        bool canCompress,
        bool processCompressInfo,
        bool processVisibilityInfo)
{
    channel.processCompressInfo = processCompressInfo;
    channel.processVisibilityInfo = processVisibilityInfo;

    float scaleCompressionError,
          positionCompressionError,
          rotationCompressionError;
    if( processCompressInfo ) {
        file.read( scaleCompressionError );
        file.read( positionCompressionError );
        file.read( rotationCompressionError );

        channel.scaleCompressionError    = scaleCompressionError;
        channel.positionCompressionError = positionCompressionError;
        channel.rotationCompressionError = rotationCompressionError;
    }

    InterpolatedChannel::ScaleKeyframes      scaleKeys      ;
    InterpolatedChannel::PositionKeyframes   positionKeys   ;
    InterpolatedChannel::RotationKeyframes   rotationKeys   ;
    InterpolatedChannel::VisibilityKeyframes visibilityKeys ;

    std::vector< uint32_t > scaleIndex, positionIndex, rotationIndex, visibilityIndex;

    if(!file.readSequence( scaleKeys ))     return false;
    if(!file.readSequence( positionKeys ))  return false;
    if(!file.readSequence( rotationKeys ))  return false;
    if(processVisibilityInfo && !file.readSequence( visibilityKeys )) return false;
    if(!file.readSequence( scaleIndex ))    return false;
    if(!file.readSequence( positionIndex )) return false;
    if(!file.readSequence( rotationIndex )) return false;
    if(processVisibilityInfo && !file.readSequence( visibilityIndex )) return false;

    channel.scaleKeys      = scaleKeys;
    channel.positionKeys   = positionKeys;
    channel.rotationKeys   = rotationKeys;
    channel.visibilityKeys = visibilityKeys;

    return true;
}

static bool writeInterpolatedAnimation(
        OBinaryFile &file,
        InterpolatedChannel const& channel)
{
    if(channel.processCompressInfo) {
        file.write( channel.scaleCompressionError );
        file.write( channel.positionCompressionError );
        file.write( channel.rotationCompressionError );
    }
    std::vector< uint32_t > scaleIndex, positionIndex, rotationIndex, visibilityIndex;

    if(!file.writeSequence(channel.scaleKeys)) return false;
    if(!file.writeSequence(channel.positionKeys)) return false;
    if(!file.writeSequence(channel.rotationKeys)) return false;
    if(channel.processVisibilityInfo && !file.writeSequence(channel.visibilityKeys))
        return false;
    if(!file.writeSequence(scaleIndex))return false;
    if(!file.writeSequence(positionIndex))return false;
    if(!file.writeSequence(rotationIndex))return false;
    if(channel.processVisibilityInfo && !file.writeSequence(visibilityIndex))
        return false;

    return true;
}

bool InterpolatedAnimation::read(IBinaryFile &file)
{
    file.read(this->totalTime);
    file.read(this->identifier);
    file.read(this->internalIdentifier);
    file.read(this->nChannels);
    for(int cb = 0; cb < this->nChannels; ++cb ) {
        InterpolatedChannel channel;
        file.read(channel.type);
        file.read(channel.channel);
        switch(channel.type) {
        case 1:
            if(!readInterpolatedAnimation(file, channel, true, false, false))
                return false;
            break;
        case 3:
            if(!readInterpolatedAnimation(file, channel, false, false, false))
                return false;
            break;
        case 4:
            if(!readInterpolatedAnimation(file, channel, true, true, false))
                return false;
            break;
        case 2004:
            if(!readInterpolatedAnimation(file, channel, false, false, true))
                return false;
            break;
        case 2005:
            if(!readInterpolatedAnimation(file, channel, false, false, true))
                return false;
            break;
        case 2: {
            std::vector<float> influences;
            file.readSequence(influences);
            fprintf(stderr, "morph animation channel %s skipped\n", channel.channel.c_str());
            break;
        }
        default:
            fprintf(stderr,"unsupported channel format :%d\n", channel.type);
            return false;
        }
        channels.push_back(std::make_pair(channel.channel, channel));
    }
    return true;
}

bool InterpolatedAnimation::write(OBinaryFile &file)
{
    file.write(this->totalTime);
    file.write(this->identifier);
    file.write(this->internalIdentifier);
    file.write(this->nChannels);
    for(int cb = 0; cb < this->nChannels; ++cb) {
        InterpolatedChannel const& channel = channels[cb].second;
        file.write(channel.type);
        file.write(channel.channel);
        if(!writeInterpolatedAnimation(file, channel))
            return false;
    }
    return true;
}

InterpolatedAnimation::InterpolatedAnimation(std::string const& fn) {
    IBinaryFile f(fn);
    this->read(f);
}
