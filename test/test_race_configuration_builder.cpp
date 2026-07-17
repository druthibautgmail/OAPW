#include <cassert>

#include "oapw/config/race_configuration_builder.hpp"

int main()
{
    using namespace oapw;

    core::GeometryParameters parameters{
        1.76,
        3.20,
        0.18,
        343.2
    };

    core::GeometryModel geometry(parameters);

    config::RaceConfigurationBuilder builder;

    auto config = builder.build(
        geometry,
        48000.0);

    assert(config.gainMatrix.leftToLeft() == 1.0f);
    assert(config.gainMatrix.rightToRight() == 1.0f);

    assert(config.gainMatrix.leftToRight() > 0.0f);
    assert(config.gainMatrix.leftToRight() < 1.0f);

    assert(config.gainMatrix.rightToLeft() > 0.0f);
    assert(config.gainMatrix.rightToLeft() < 1.0f);

    assert(config.crossDelaySeconds == 0.0);

    assert(config.recursionOrder == 16u);

    return 0;
}
