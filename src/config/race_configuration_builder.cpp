#include "oapw/config/race_configuration_builder.hpp"

namespace oapw::config
{

//RaceConfiguration RaceConfigurationBuilder::build(
//    const core::GeometryModel& geometry) const

RaceConfiguration RaceConfigurationBuilder::build(
    const core::GeometryModel& geometry,
    double sampleRate) const

{
    (void)sampleRate;

    RaceConfiguration config;

    auto result = geometry.calculate();

    config.gainMatrix =
        acousticModel_.calculateGainMatrix(result);

    config.crossDelaySeconds = 0.0;

    config.crossGainLinear =
        config.gainMatrix.leftToRight();

    config.recursionOrder = 16;

    return config;
}

}
