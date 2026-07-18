#include "oapw/core/race_processor.hpp"

namespace oapw::core
{

void RaceProcessor::configure(
    const config::RaceConfiguration& configuration)
{
    setGainMatrix(configuration.gainMatrix);

    if (sampleRate_ <= 0.0)
    {
        return;
    }

    const double delaySamples =
        configuration.crossDelaySeconds * sampleRate_;

    setDelaySamples(
        delaySamples,
        delaySamples);
}

void RaceProcessor::setGainMatrix(
    const GainMatrix& gains)
{
    gainMatrix_ = gains;
}

void RaceProcessor::prepare(
    double sampleRate,
    std::size_t maximumDelaySamples)
{
    sampleRate_ = sampleRate;

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

void RaceProcessor::process(
    float inputLeft,
    float inputRight,
    float& outputLeft,
    float& outputRight)
{
    crossLeft_ = 0.0f;
    crossRight_ = 0.0f;

    crossfeed_.process(
        inputLeft,
        inputRight,
        gainMatrix_,
        crossLeft_,
        crossRight_);

    leftDelay_.process(
        &crossLeft_,
        &delayedLeft_,
        1);

    rightDelay_.process(
        &crossRight_,
        &delayedRight_,
        1);

    outputLeft = delayedLeft_;
    outputRight = delayedRight_;
}

} // namespace oapw::core
