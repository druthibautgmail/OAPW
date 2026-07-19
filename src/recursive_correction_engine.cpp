#include "oapw/core/recursive_correction_engine.hpp"

namespace oapw::core
{

void RecursiveCorrectionEngine::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
}

void RecursiveCorrectionEngine::reset()
{
}

void RecursiveCorrectionEngine::configure(
    const config::RaceConfiguration& configuration)
{
    configuration_ = configuration;
}

void RecursiveCorrectionEngine::process(
    float inputLeft,
    float inputRight,
    float& correctionLeft,
    float& correctionRight)
{
    // Milestone 0.3.1:
    // Noch keine Rekursion.
    // Bypass liefert keine zusätzlichen Korrektursignale.

    (void)inputLeft;
    (void)inputRight;

    correctionLeft = 0.0f;
    correctionRight = 0.0f;
}

} // namespace oapw::core
