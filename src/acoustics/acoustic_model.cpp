#include "oapw/acoustics/acoustic_model.hpp"

namespace oapw::acoustics
{

core::GainMatrix AcousticModel::calculateGainMatrix(
    const core::GeometryResult& geometry) const
{
    core::GainMatrix gains;

    constexpr float directGain = 1.0f;

    const float leftCrossGain =
        static_cast<float>(geometry.dLL / geometry.dRL);

    const float rightCrossGain =
        static_cast<float>(geometry.dRR / geometry.dLR);

    gains.setGains(
        directGain,
        directGain,
        leftCrossGain,
        rightCrossGain);

    return gains;
}

} // namespace oapw::acoustics
