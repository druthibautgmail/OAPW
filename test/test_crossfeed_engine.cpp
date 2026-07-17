#include <cassert>
#include <cmath>
#include <iostream>

#include "oapw/core/crossfeed_engine.hpp"
#include "oapw/core/gain_matrix.hpp"

using namespace oapw::core;

int main()
{
    GainMatrix gains;
    gains.setGains(
        1.0f,   // LL
        1.0f,   // RR
        0.2f,   // LR
        0.3f);  // RL

    CrossfeedEngine crossfeed;

    float outLeft{};
    float outRight{};

    crossfeed.process(
        1.0f,   // Left in
        0.5f,   // Right in
        gains,
        outLeft,
        outRight);

    constexpr float eps = 1e-6f;

    assert(std::fabs(outLeft  - 1.15f) < eps);
    assert(std::fabs(outRight - 0.70f) < eps);

    std::cout << "CrossfeedEngine test passed.\n";

    return 0;
}
