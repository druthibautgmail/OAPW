#include "oapw/acoustics/acoustic_model.hpp"

namespace oapw::acoustics
{

core::GainMatrix AcousticModel::calculateGainMatrix(
    const core::GeometryResult&) const
{
    return {};
}

} // namespace oapw::acoustics
