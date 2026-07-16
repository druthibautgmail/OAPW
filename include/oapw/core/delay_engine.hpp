#pragma once

#include <cstddef>
#include <vector>

namespace oapw::core
{

class DelayEngine
{
public:

    DelayEngine();

    void prepare(double sampleRate,
                 std::size_t maximumDelaySamples);

    void reset();

    void setDelay(double delaySamples);

    void process(const float* input,
                 float* output,
                 std::size_t numSamples);

private:

    double sampleRate_{0.0};

    double delaySamples_{0.0};

    std::size_t maximumDelay_{0};

    std::size_t writeIndex_{0};

    std::vector<float> buffer_;
};

}
