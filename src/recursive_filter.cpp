#include "oapw/core/recursive_filter.hpp"

namespace oapw::core
{

void RecursiveFilter::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
}

void RecursiveFilter::reset()
{
    // Noch keine internen Zustände vorhanden.
}

float RecursiveFilter::process(float input)
{
    // MVP: Bypass
    return input;
}

} // namespace oapw::core
