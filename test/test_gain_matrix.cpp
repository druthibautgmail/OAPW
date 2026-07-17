#include <cassert>
#include <iostream>

#include "oapw/core/gain_matrix.hpp"

using namespace oapw::core;

int main()
{
    GainMatrix gains;

    gains.setGains(
        1.0f,
        0.95f,
        0.18f,
        0.22f);

    assert(gains.leftToLeft()  == 1.0f);
    assert(gains.rightToRight() == 0.95f);
    assert(gains.leftToRight() == 0.18f);
    assert(gains.rightToLeft() == 0.22f);

    std::cout << "GainMatrix test passed.\n";

    return 0;
}
