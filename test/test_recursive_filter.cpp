#include "oapw/core/recursive_filter.hpp"

#include <cassert>

using namespace oapw::core;

int main()
{
    RecursiveFilter filter;

    filter.prepare(44100.0);
    filter.reset();

    constexpr float input = 0.5f;
    const float output = filter.process(input);

    assert(output == input);

    return 0;
}
