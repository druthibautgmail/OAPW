#include <iostream>
#include <vector>
#include <string>

#include "oapw/io/wav_reader.hpp"
#include "oapw/io/wav_writer.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr
            << "Usage:\n"
            << "  oapw_offline_demo <input.wav> <output.wav>\n";
        return 1;
    }

    const std::string inputFile  = argv[1];
    const std::string outputFile = argv[2];

    using namespace oapw::io;

    WavReader reader;

    if (!reader.open(inputFile))
    {
        std::cerr << "Cannot open " << inputFile << '\n';
        return 1;
    }

    std::cout << "Input file successfully loaded\n";
    std::cout << "Sample rate : " << reader.sampleRate() << '\n';
    std::cout << "Channels    : " << reader.channelCount() << '\n';
    std::cout << "Bits/sample : " << reader.bitsPerSample() << '\n';
    std::cout << "Frames      : " << reader.sampleCount() << '\n';

    std::vector<StereoSample> output = reader.samples();

    WavWriter writer;

    if (!writer.write(
            outputFile,
            output,
            reader.sampleRate()))
    {
        std::cerr << "Cannot write " << outputFile << '\n';
        return 1;
    }

    std::cout << outputFile << " successfully written.\n";

    return 0;
}
