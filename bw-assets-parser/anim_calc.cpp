/*
anim_calc.cpp
Birthday : 10/3/2014 12:17:17 AM
Author   : If
*/

#include "anim_calc.h"
#include <assert.h>

static const int INVALID_CHANNEL_TYPE = -1;

static InterpolatedChannel invalidAnimChannel() {
    InterpolatedChannel channel;
    channel.type = INVALID_CHANNEL_TYPE;
    return channel;
}

PreparedInterpolatedAnimation::PreparedInterpolatedAnimation(
        InterpolatedAnimation const& anim, VisualAsset const& visual)
{
    for(Node const& node:visual.nodeTree.nodes) {
        bool channelFound = false;
        for(auto const& channel:anim.channels) {
            if(channel.second.channel == node.identifier) {
                sortedAnimChannels_.push_back(channel.second);
                channelFound = true;
                break;
            }
        }
        if(!channelFound) {
            sortedAnimChannels_.push_back(invalidAnimChannel());
        }
    }
    assert(sortedAnimChannels_.size() == visual.nodeTree.nodes.size());
}

static Transform calc(InterpolatedChannel const& channel, float t)
{
    Transform trans;
    size_t i=0;
    if (channel.type == INVALID_CHANNEL_TYPE) {
        return trans;
    }

    if(t <= 0) {
        trans.scale = channel.scaleKeys[0].second;
        trans.rotate = channel.rotationKeys[0].second;
        trans.translate = channel.positionKeys[0].second;
        return trans;
    }

    for(; i<channel.scaleKeys.size(); ++i) {
        if(channel.scaleKeys[i].first >= t)
            break;
    }
    if (channel.scaleKeys.back().first <= t) {
        trans.scale = channel.scaleKeys.back().second;
    } else if(i>0 && i<channel.scaleKeys.size() && channel.scaleKeys[i].first >= t) {
        trans.scale = lerp( channel.scaleKeys[i-1].second, channel.scaleKeys[i].second,
                            (t-channel.scaleKeys[i-1].first)/(channel.scaleKeys[i].first-channel.scaleKeys[i-1].first));
    } else {
        // what?
        fprintf(stderr, "what???\n");
    }

    for(i=0; i<channel.positionKeys.size(); ++i) {
        if(channel.positionKeys[i].first >= t)
            break;
    }
    if (channel.positionKeys.back().first <= t) {
        trans.translate = channel.positionKeys.back().second;
    } else if(i>0 && i<channel.positionKeys.size() && channel.positionKeys[i].first >= t) {
        trans.translate = lerp( channel.positionKeys[i-1].second, channel.positionKeys[i].second,
                                (t-channel.positionKeys[i-1].first)/(channel.positionKeys[i].first-channel.positionKeys[i-1].first));
    } else {
        // what?
        fprintf(stderr, "what???\n");
    }

    for(i=0; i<channel.rotationKeys.size(); ++i) {
        if(channel.rotationKeys[i].first >= t)
            break;
    }
    if (channel.rotationKeys.back().first <= t) {
        trans.rotate = channel.rotationKeys.back().second;
    } else if(i>0 && i<channel.rotationKeys.size() && channel.rotationKeys[i].first >= t) {
        trans.rotate = slerp( channel.rotationKeys[i-1].second, channel.rotationKeys[i].second, 
                             (t-channel.rotationKeys[i-1].first)/(channel.rotationKeys[i].first - channel.rotationKeys[i-1].first));
    } else {
        // what?
        fprintf(stderr, "what???\n");
    }
    return trans;
}

void PreparedInterpolatedAnimation::result(std::vector<Transform>& receiver, float t) const
{
    assert(receiver.size() == sortedAnimChannels_.size());
    for(size_t i=0; i<sortedAnimChannels_.size(); ++i) {
        if(sortedAnimChannels_[i].type == INVALID_CHANNEL_TYPE)
            continue;
        receiver[i] = calc(sortedAnimChannels_[i], t);
    }
}
