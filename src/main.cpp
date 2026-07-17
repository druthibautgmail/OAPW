#include <array>
#include <iostream>

#include "oapw/core/delay_engine.hpp"

int main()
{
    constexpr std::size_t N = 12;

    const std::array<double,5> integerDelays =
    {
        0.0,
        1.0,
        3.0,
        7.0,
        10.0
    };

    const std::array<double,3> fractionalDelays =
    {
        3.25,
        3.50,
        3.75
    };

    for (double d : integerDelays)
    {
        oapw::core::DelayEngine delay;

        delay.prepare(48000.0, 16);
        delay.setDelay(d);

        std::array<float, N> input =
        {
            1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f, 0.f, 0.f
        };

        std::array<float, N> output{};

        delay.process(input.data(), output.data(), N);

        std::cout << "Delay = "
                  << d
                  << " samples\n";

        std::cout << "Output: ";

        for (float s : output)
            std::cout << s << ' ';

        std::cout << "\n\n";
    }

    std::cout << "=============================\n";
    std::cout << "Fractional Delay Tests\n";
    std::cout << "=============================\n\n";

    for (double d : fractionalDelays)
    {
        oapw::core::DelayEngine delay;

        delay.prepare(48000.0, 16);
        delay.setDelay(d);

        std::array<float, N> input =
        {
            1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f, 0.f, 0.f
        };

        std::array<float, N> output{};

        delay.process(input.data(), output.data(), N);

        std::cout << "Delay = "
                  << d
                  << " samples\n";

        std::cout << "Output: ";

        for (float s : output)
            std::cout << s << ' ';

        std::cout << "\n\n";
    }

    return 0;
}
