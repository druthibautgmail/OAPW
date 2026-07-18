#include "oapw/core/recursive_filter.hpp"

namespace oapw::core
{

void RecursiveFilter::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
}

void RecursiveFilter::reset()
{
    x1_ = 0.0f;
    y1_ = 0.0f;
}

void RecursiveFilter::setCoefficients(
    float b0,
    float b1,
    float a1)
{
    b0_ = b0;
    b1_ = b1;
    a1_ = a1;
}

float RecursiveFilter::process(float input)
{
    const float output =
        b0_ * input +
        b1_ * x1_ -
        a1_ * y1_;

    x1_ = input;
    y1_ = output;

    return output;
}

} // namespace oapw::core
