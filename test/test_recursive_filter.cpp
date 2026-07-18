#include "oapw/core/recursive_filter.hpp"

#include <cassert>
#include <cmath>

using namespace oapw::core;

namespace
{
constexpr float epsilon = 1.0e-6f;

bool almostEqual(float a, float b)
{
    return std::fabs(a - b) < epsilon;
}
}

int main()
{
    RecursiveFilter filter;

    filter.prepare(44100.0);

    //
    // Test 1:
    // Nach reset() muss ein Nullsample auch Null ergeben.
    //
    filter.reset();

    assert(almostEqual(filter.process(0.0f), 0.0f));

    //
    // Test 2:
    // Impulsantwort eines rekursiven Filters 1. Ordnung.
    //
    filter.reset();

    filter.setCoefficients(
        1.0f,   // b0
        0.0f,   // b1
        0.5f);  // a1

    const float expected[] =
    {
         1.0f,
        -0.5f,
         0.25f,
        -0.125f,
         0.0625f
    };

    assert(almostEqual(filter.process(1.0f), expected[0]));

    for (int i = 1; i < 5; ++i)
    {
        assert(almostEqual(
            filter.process(0.0f),
            expected[i]));
    }

    //
    // Test 3:
    // Nach reset() muss dieselbe Impulsantwort erneut entstehen.
    //
    filter.reset();

    assert(almostEqual(filter.process(1.0f), expected[0]));

    for (int i = 1; i < 5; ++i)
    {
        assert(almostEqual(
            filter.process(0.0f),
            expected[i]));
    }

    return 0;
}
