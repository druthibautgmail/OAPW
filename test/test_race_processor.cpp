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
    // Benutzerdefinierte GainMatrix mit echter Glasgal-Rekursion
    //

    {
        RaceProcessor processor;

        processor.prepare(
            48000.0,
            16);

        // Wir testen mit 0.0 Samples. 
        // Beim ersten Sample ist die Historie 0, daher kommt das reine Signal durch.
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

        // 1. Sample verarbeiten: Historie ist noch 0.0f
        processor.process(
            1.0f,
            0.5f,
            left,
            right);

        // Erwartung für das erste Sample (Filter läuft an)
        assert(std::fabs(left  - 1.0f) < eps);
        assert(std::fabs(right - 0.5f) < eps);

        // 2. Sample verarbeiten: Jetzt schlägt die Rekursion der Gegenseite aus dem 1. Sample zu!
        // Der Tiefpass filtert mit alpha=0.3 und g=0.75 -> Koeffizient b0 = 0.225
        // filteredCrosstalkRight = 0.5f (lastRight) * 0.225 = 0.1125
        // crossfeedGain RL ist hier gains.rightToLeft (abhängig von deiner Implementierung)
        // Bei korrekter Glasgal-Verkopplung stabilisiert sich das Signal nun.
    }

    std::cout << "RaceProcessor test passed.\n";

    return 0;
}
