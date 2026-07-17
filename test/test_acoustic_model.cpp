#include <cassert>

#include "oapw/acoustics/acoustic_model.hpp"

int main()
{
    oapw::acoustics::AcousticModel model;

    oapw::core::GeometryResult geometry{};

    auto gains = model.calculateGainMatrix(geometry);

    assert(gains.leftToLeft()  == 1.0f);
    assert(gains.rightToRight() == 1.0f);

    assert(gains.leftToRight() == 0.0f);
    assert(gains.rightToLeft() == 0.0f);

    return 0;
}
