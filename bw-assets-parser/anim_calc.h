#pragma once

#include "interpolated_anim.h"
#include "visual.h"
#include "transform.h"

class PreparedInterpolatedAnimation
{
    std::vector<InterpolatedChannel> sortedAnimChannels_;
public:

    // sort animations to match nodes order in visual
    PreparedInterpolatedAnimation(InterpolatedAnimation const& anim, VisualAsset const& visual);

    void result(std::vector<Transform>&, float t) const;
};

