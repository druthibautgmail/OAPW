#include "oapw/config/race_configuration_builder.hpp"

namespace oapw::config
{

RaceConfiguration RaceConfigurationBuilder::build(
    const core::GeometryModel& geometry,
    double sampleRate) const
{
    (void)sampleRate;

    RaceConfiguration config;

    const auto result = geometry.calculate();

    constexpr float compensationFactor = 0.5f;

    // Physikalische Gain-Matrix berechnen
    auto gains =
        acousticModel_.calculateGainMatrix(result);

    // DSP-Kompensation anwenden
    gains.setGains(
        gains.leftToLeft(),
        gains.rightToRight(),
        gains.leftToRight() * compensationFactor,
        gains.rightToLeft() * compensationFactor);

    config.gainMatrix = gains;

    constexpr double crossDelayMicroseconds = 68.0;

    config.crossDelaySeconds =
        crossDelayMicroseconds * 1.0e-6;

    config.compensationFactor = compensationFactor;

    config.crossGainLinear =
        gains.leftToRight();

    config.recursionOrder = 16;

    return config;
}

} // namespace oapw::config
