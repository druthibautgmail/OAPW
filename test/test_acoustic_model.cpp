#include <cassert>

#include "oapw/acoustics/acoustic_model.hpp"

int main()
{
    oapw::acoustics::AcousticModel model;

    oapw::core::GeometryResult geometry{};

    geometry.dLL = 3.20;
    geometry.dRR = 3.20;

    geometry.dLR = 3.65;
    geometry.dRL = 3.65;

    auto gains = model.calculateGainMatrix(geometry);

    assert(gains.leftToLeft() == 1.0f);
    assert(gains.rightToRight() == 1.0f);

    assert(gains.leftToRight() > 0.0f);
    assert(gains.leftToRight() < 1.0f);

    assert(gains.rightToLeft() > 0.0f);
    assert(gains.rightToLeft() < 1.0f);

    return 0;
}
