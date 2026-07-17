#include <iostream>
#include <array>

#include "oapw/core/delay_engine.hpp"

int main()
{
    constexpr std::size_t N = 12;

    const std::array<double,5> delays =
    {
        0.0,
        1.0,
        3.0,
        7.0,
        10.0
    };

    for (double d : delays)
    {
        oapw::core::DelayEngine delay;

        delay.prepare(48000.0,16);
        delay.setDelay(d);
        std::array<float,N> input =
        {
            1.f,0.f,0.f,0.f,0.f,0.f,
            0.f,0.f,0.f,0.f,0.f,0.f
        };

        std::array<float,N> output{};

        delay.process(input.data(),output.data(),N);

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
