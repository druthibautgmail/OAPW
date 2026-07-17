#include "oapw/core/crossfeed_engine.hpp"

namespace oapw::core
{

void CrossfeedEngine::process(
    float inputLeft,
    float inputRight,
    const GainMatrix& gains,
    float& outputLeft,
    float& outputRight) const
{
    outputLeft =
        gains.leftToLeft() * inputLeft +
        gains.rightToLeft() * inputRight;

    outputRight =
        gains.rightToRight() * inputRight +
        gains.leftToRight() * inputLeft;
}

}
