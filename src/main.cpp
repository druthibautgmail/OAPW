#include <iostream>

#include "oapw/core/geometry_model.hpp"
#include "oapw/core/delay_engine.hpp"

using namespace oapw::core;

int main()
{
    std::cout << '\n';
    std::cout << "=====================================\n";
    std::cout << "      OAPW-Core Version 0.1\n";
    std::cout << " Open Ambiophony for PipeWire\n";
    std::cout << "=====================================\n\n";

    GeometryParameters parameters;

    parameters.speakerDistance   = 1.76;
    parameters.listeningDistance = 3.20;
    // parameters.earDistance bleibt auf dem Standardwert von 0.18 m

    GeometryModel model(parameters);

    auto geometry = model.calculate();

    std::cout
        << "Left speaker : ("
        << geometry.leftSpeaker.x
        << ", "
        << geometry.leftSpeaker.y
        << ")\n";

    std::cout
        << "Right speaker: ("
        << geometry.rightSpeaker.x
        << ", "
        << geometry.rightSpeaker.y
        << ")\n";

    std::cout
        << "Head         : ("
        << geometry.listener.head.x
        << ", "
        << geometry.listener.head.y
        << ")\n";

    std::cout
        << "Left ear     : ("
        << geometry.listener.leftEar.x
        << ", "
        << geometry.listener.leftEar.y
        << ")\n";

    std::cout
        << "Right ear    : ("
        << geometry.listener.rightEar.x
        << ", "
        << geometry.listener.rightEar.y
        << ")\n";

    std::cout << "\n";

    std::cout << "Distances\n";
    std::cout << "---------\n";

    std::cout << "Left speaker -> Left ear : "
              << geometry.dLL
              << " m\n";

    std::cout << "Left speaker -> Right ear: "
              << geometry.dLR
              << " m\n";

    std::cout << "Right speaker -> Left ear: "
              << geometry.dRL
              << " m\n";

    std::cout << "Right speaker -> Right ear: "
              << geometry.dRR
              << " m\n";

    std::cout << "\n";

    std::cout << "Delay Parameters\n";
    std::cout << "----------------\n";

    std::cout << "Delta distance : "
              << geometry.deltaDistance
              << " m\n";

    std::cout << "Delay          : "
              << geometry.delayMicroseconds
              << " us\n";

    std::cout << "Samples 44.1k  : "
              << geometry.delaySamples44k1
              << "\n";

    std::cout << "Samples 48k    : "
              << geometry.delaySamples48k
              << "\n";

    std::cout << "Samples 96k    : "
              << geometry.delaySamples96k
              << "\n";
    return 0;
}
