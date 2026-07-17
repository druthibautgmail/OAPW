#include "oapw/core/race_processor.hpp"

namespace oapw::core
{

void RaceProcessor::setGainMatrix(
    const GainMatrix& gains)
{
    gainMatrix_ = gains;
}

void RaceProcessor::prepare(
    double sampleRate,
    std::size_t maximumDelaySamples)
{
    leftDelay_.prepare(
        sampleRate,
        maximumDelaySamples);

    rightDelay_.prepare(
        sampleRate,
        maximumDelaySamples);
}

void RaceProcessor::setDelaySamples(
    double leftDelaySamples,
    double rightDelaySamples)
{
    leftDelay_.setDelay(
        leftDelaySamples);

    rightDelay_.setDelay(
        rightDelaySamples);
}

/*void RaceProcessor::process(
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
}*/

void RaceProcessor::process(
    float inputLeft,
    float inputRight,
    float& outputLeft,
    float& outputRight)
{
    float mixedLeft{};
    float mixedRight{};

    crossfeed_.process(
        inputLeft,
        inputRight,
        gainMatrix_,
        mixedLeft,
        mixedRight);

    leftDelay_.process(
        &mixedLeft,
        &outputLeft,
        1);

    rightDelay_.process(
        &mixedRight,
        &outputRight,
        1);
}

} // namespace oapw::core
