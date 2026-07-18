#include <cassert>
#include <cmath>
#include <iostream>

#include "oapw/core/gain_matrix.hpp"
#include "oapw/core/race_processor.hpp"

using namespace oapw::core;

int main()
{
    constexpr float eps = 1e-6f;

    //
    // Test 1:
    // Standard-GainMatrix (Identität)
    //
    {
        RaceProcessor processor;

        processor.prepare(
            48000.0,
            16);

        processor.setDelaySamples(
            0.0,
            0.0);

        float left{};
        float right{};

        processor.process(
            1.0f,
            0.5f,
            left,
            right);

        assert(std::fabs(left  - 1.0f) < eps);
        assert(std::fabs(right - 0.5f) < eps);
    }

    //
    // Test 2:
    // Benutzerdefinierte GainMatrix
    //
    {
        RaceProcessor processor;

        processor.prepare(
            48000.0,
            16);

        processor.setDelaySamples(
            0.0,
            0.0);

        GainMatrix gains;

        gains.setGains(
            1.0f,   // LL
            1.0f,   // RR
            0.2f,   // LR
            0.3f);  // RL

        processor.setGainMatrix(gains);

        float left{};
        float right{};

        processor.process(
            1.0f,
            0.5f,
            left,
            right);

	assert(std::fabs(left  - 0.80f) < eps);
	assert(std::fabs(right - 0.35f) < eps);
    }

    std::cout << "RaceProcessor test passed.\n";

    return 0;
}
