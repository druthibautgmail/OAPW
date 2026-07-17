#include "oapw/core/delay_engine.hpp"
#include <algorithm>

namespace oapw::core
{

DelayEngine::DelayEngine() = default;

void DelayEngine::prepare(double sampleRate,
                          std::size_t maximumDelaySamples)
{
    sampleRate_ = sampleRate;

    maximumDelay_ = maximumDelaySamples;

    delaySamples_ = 0.0;

    writeIndex_ = 0;

    buffer_.assign(maximumDelay_ + 1, 0.0f);
}

void DelayEngine::reset()
{
    std::fill(buffer_.begin(),
              buffer_.end(),
              0.0f);

    writeIndex_ = 0;
}

void DelayEngine::setDelay(double delaySamples)
{
    delaySamples_ = std::clamp(
        delaySamples,
        0.0,
        static_cast<double>(maximumDelay_));
}

void DelayEngine::process(const float* input,
                          float* output,
                          std::size_t numSamples)
{
    const std::size_t bufferSize = buffer_.size();

    const double delay = delaySamples_;

    const std::size_t integerDelay =
        static_cast<std::size_t>(delay);

    const double fraction =
        delay - static_cast<double>(integerDelay);
    (void)fraction;
for (std::size_t i = 0; i < numSamples; ++i)
    {
        // aktuelles Eingangssample speichern
        buffer_[writeIndex_] = input[i];

        // Leseposition berechnen
        const std::size_t index0 =
            (writeIndex_
             + bufferSize
             - integerDelay)
            % bufferSize;

        const std::size_t index1 =
            (writeIndex_
             + bufferSize
             - integerDelay
             - 1)
            % bufferSize;

        // verzögertes Sample ausgeben
//        output[i] = buffer_[readIndex];
        const float s0 = buffer_[index0];
        const float s1 = buffer_[index1];

        output[i] =
            static_cast<float>(
                (1.0 - fraction) * s0 +
                fraction        * s1);
        // Schreibindex weiterschalten
        writeIndex_++;

        if (writeIndex_ >= bufferSize)
            writeIndex_ = 0;
    }

}
}
