#include "oapw/core/gain_matrix.hpp"

namespace oapw::core
{

void GainMatrix::setGains(
    float leftToLeft,
    float rightToRight,
    float leftToRight,
    float rightToLeft)
{
    gainLL_ = leftToLeft;
    gainRR_ = rightToRight;

    gainLR_ = leftToRight;
    gainRL_ = rightToLeft;
}

float GainMatrix::leftToLeft() const
{
    return gainLL_;
}

float GainMatrix::rightToRight() const
{
    return gainRR_;
}

float GainMatrix::leftToRight() const
{
    return gainLR_;
}

float GainMatrix::rightToLeft() const
{
    return gainRL_;
}

}
