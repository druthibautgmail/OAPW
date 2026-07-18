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
    // Nur den zu kompensierenden Übersprechanteil berechnen.
    // Das Direktsignal wird später im RaceProcessor behandelt.

    outputLeft =
        gains.rightToLeft() * inputRight;

    outputRight =
        gains.leftToRight() * inputLeft;
}

} // namespace oapw::core
