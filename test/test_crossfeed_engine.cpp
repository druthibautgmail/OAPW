#include <cassert>
#include <cmath>
#include <iostream>

#include "oapw/core/crossfeed_engine.hpp"
#include "oapw/core/gain_matrix.hpp"

using namespace oapw::core;

int main()
{
    constexpr float eps = 1e-6f;

    //
    // Test 1:
    // Standard-GainMatrix
    //
    {
        GainMatrix gains;

        CrossfeedEngine crossfeed;

        float outLeft{};
        float outRight{};

        crossfeed.process(
            1.0f,
            0.5f,
            gains,
            outLeft,
            outRight);

        assert(std::fabs(outLeft) < eps);
        assert(std::fabs(outRight) < eps);
    }

    //
    // Test 2:
    // Benutzerdefinierte GainMatrix
    //
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

        // Nur Übersprechanteile
        //
        // outLeft  = RL * Right = 0.3 * 0.5 = 0.15
        // outRight = LR * Left  = 0.2 * 1.0 = 0.20

        assert(std::fabs(outLeft  - 0.15f) < eps);
        assert(std::fabs(outRight - 0.20f) < eps);
    }

    std::cout << "CrossfeedEngine test passed.\n";

    return 0;
}
