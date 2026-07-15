#include <iostream>

#include "geometry_model.h"

using namespace oapw::core;

int main()
{
    std::cout << '\n';
    std::cout << "=====================================\n";
    std::cout << "      OAPW-Core Version 0.1\n";
    std::cout << " Open Ambiophony for PipeWire\n";
    std::cout << "=====================================\n\n";

    GeometryParameters geometry;

    geometry.speakerDistance   = 1.76;
    geometry.listeningDistance = 3.20;

    GeometryModel model(geometry);

/*    auto result = model.calculate();

    (void)result;

    std::cout << "GeometryModel initialized successfully.\n";*/

auto left  = model.leftSpeaker();
auto right = model.rightSpeaker();
auto head  = model.listener();

std::cout << "Left speaker : ("
          << left.x
          << ", "
          << left.y
          << ")\n";

std::cout << "Right speaker: ("
          << right.x
          << ", "
          << right.y
          << ")\n";

std::cout << "Listener     : ("
          << head.x
          << ", "
          << head.y
          << ")\n";

    return 0;


}
