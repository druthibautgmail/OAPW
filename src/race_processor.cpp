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

    leftFilter_.prepare(sampleRate);
    rightFilter_.prepare(sampleRate);

    leftFilter_.reset();
    rightFilter_.reset();

    leftFilter_.setCoefficients(
        1.0f,
        0.0f,
        0.0f);

    rightFilter_.setCoefficients(
        1.0f,
        0.0f,
        0.0f);

    feedbackLeft_ = 0.0f;
    feedbackRight_ = 0.0f;
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

    const float recursiveInputLeft =
        delayedLeft_ + feedbackGain_ * feedbackLeft_;

    const float recursiveInputRight =
        delayedRight_ + feedbackGain_ * feedbackRight_;

    filteredLeft_ =
        leftFilter_.process(recursiveInputLeft);

    filteredRight_ =
        rightFilter_.process(recursiveInputRight);

    feedbackLeft_ = filteredLeft_;
    feedbackRight_ = filteredRight_;

    outputLeft = filteredLeft_;
    outputRight = filteredRight_;
}

} // namespace oapw::core
