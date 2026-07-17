#include <cassert>

#include "oapw/config/race_configuration_builder.hpp"

int main()
{
    oapw::core::GeometryParameters parameters{
        1.76,   // speakerDistance
        3.20,   // listeningDistance
        0.18,   // earDistance
        343.2   // speedOfSound
    };

    oapw::core::GeometryModel geometry(parameters);

    oapw::config::RaceConfigurationBuilder builder;

    auto config = builder.build(geometry);

    assert(config.leftDelaySamples == 0.0);
    assert(config.rightDelaySamples == 0.0);

    return 0;
}
