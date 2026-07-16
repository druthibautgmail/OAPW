#include "oapw/core/delay_engine.hpp"
#include <algorithm>

namespace oapw::core
{

DelayEngine::DelayEngine() = default;

void DelayEngine::prepare(double sampleRate,
                          std::size_t maximumDelaySamples)
{
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
