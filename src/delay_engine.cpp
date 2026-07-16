#include "oapw/core/delay_engine.hpp"
#include <algorithm>

namespace oapw::core
{

DelayEngine::DelayEngine() = default;

void DelayEngine::prepare(double sampleRate,
                          std::size_t maximumDelaySamples)
{
    sampleRate_ = sampleRate;

    maximumDelay_ = maximumDelaySamples;

    delaySamples_ = 0.0;

    writeIndex_ = 0;

    buffer_.assign(maximumDelay_ + 1, 0.0f);
}

void DelayEngine::reset()
{
}

void DelayEngine::setDelay(double delaySamples)
{
}

void DelayEngine::process(const float* input,
                          float* output,
                          std::size_t numSamples)
{
}

}
