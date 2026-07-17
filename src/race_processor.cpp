#include "oapw/core/race_processor.hpp"

namespace oapw::core
{

void RaceProcessor::setGainMatrix(
    const GainMatrix& gains)
{
    gainMatrix_ = gains;
}

void RaceProcessor::process(
    float inputLeft,
    float inputRight,
    float& outputLeft,
    float& outputRight)
{
    crossfeed_.process(
        inputLeft,
        inputRight,
        gainMatrix_,
        outputLeft,
        outputRight);
}

} // namespace oapw::core
