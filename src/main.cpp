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

    auto result = model.calculate();

    (void)result;

    std::cout << "GeometryModel initialized successfully.\n";

    return 0;
}
