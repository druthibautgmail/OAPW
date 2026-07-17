#include "oapw/core/delay_engine.hpp"

#include <cassert>

using namespace oapw::core;

void testDelay0Samples()
{
    DelayEngine delay;

    delay.prepare(48000.0, 16);
    delay.setDelay(0.0);

    const float input[] = {1.f, 2.f, 3.f, 4.f, 5.f};
    float output[5] = {};

    delay.process(input, output, 5);

    assert(output[0] == 1.f);
    assert(output[1] == 2.f);
    assert(output[2] == 3.f);
    assert(output[3] == 4.f);
    assert(output[4] == 5.f);
}

void testDelay1Sample()
{
    DelayEngine delay;

    delay.prepare(48000.0, 16);
    delay.setDelay(1.0);

    const float input[] = {1.f, 2.f, 3.f, 4.f, 5.f};
    float output[5] = {};

    delay.process(input, output, 5);

    assert(output[0] == 0.f);
    assert(output[1] == 1.f);
    assert(output[2] == 2.f);
    assert(output[3] == 3.f);
    assert(output[4] == 4.f);
}

void testDelay3Samples()
{
    DelayEngine delay;

    delay.prepare(48000.0, 16);
    delay.setDelay(3.0);

    const float input[] = {1.f, 2.f, 3.f, 4.f, 5.f};
    float output[5] = {};

    delay.process(input, output, 5);

    assert(output[0] == 0.f);
    assert(output[1] == 0.f);
    assert(output[2] == 0.f);
    assert(output[3] == 1.f);
    assert(output[4] == 2.f);
}

void testReset()
{
    DelayEngine delay;

    delay.prepare(48000.0, 16);
    delay.setDelay(3.0);

    const float input[] = {1.f, 2.f, 3.f, 4.f, 5.f};
    float output[5] = {};

    delay.process(input, output, 5);

    delay.reset();

    const float zeros[] = {0.f, 0.f, 0.f, 0.f, 0.f};

    delay.process(zeros, output, 5);

    assert(output[0] == 0.f);
    assert(output[1] == 0.f);
    assert(output[2] == 0.f);
    assert(output[3] == 0.f);
    assert(output[4] == 0.f);
}
int main()
{
    testDelay0Samples();
    testDelay1Sample();
    testDelay3Samples();
    testReset();

    return 0;
}
