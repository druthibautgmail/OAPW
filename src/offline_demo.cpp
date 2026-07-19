#include <iostream>
#include <string>
#include <vector>

#include "oapw/acoustics/acoustic_model.hpp"
#include "oapw/config/race_configuration_builder.hpp"
#include "oapw/core/geometry_model.hpp"
#include "oapw/core/race_processor.hpp"
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
    using namespace oapw::core;
    using namespace oapw::config;

    //------------------------------------------------------------
    // Eingabedatei öffnen
    //------------------------------------------------------------

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

    //------------------------------------------------------------
    // Geometrie
    //------------------------------------------------------------

    GeometryParameters geometryParameters;

    geometryParameters.speakerDistance   = 1.76;
    geometryParameters.listeningDistance = 3.20;
    geometryParameters.earDistance       = 0.18;
    geometryParameters.speedOfSound      = 343.0;

    GeometryModel geometry(geometryParameters);

    //------------------------------------------------------------
    // DSP konfigurieren
    //------------------------------------------------------------

    RaceConfigurationBuilder builder;

    auto configuration =
        builder.build(
            geometry,
            reader.sampleRate());

std::cout << "\n=== Geometry ===\n";

const auto geometryResult = geometry.calculate();

std::cout << "dLL = " << geometryResult.dLL << " m\n";
std::cout << "dLR = " << geometryResult.dLR << " m\n";
std::cout << "dRL = " << geometryResult.dRL << " m\n";
std::cout << "dRR = " << geometryResult.dRR << " m\n";

std::cout << "\n=== Race Configuration ===\n";

std::cout << "Gain LL = "
          << configuration.gainMatrix.leftToLeft()
          << '\n';

std::cout << "Gain LR = "
          << configuration.gainMatrix.leftToRight()
          << '\n';

std::cout << "Gain RL = "
          << configuration.gainMatrix.rightToLeft()
          << '\n';

std::cout << "Gain RR = "
          << configuration.gainMatrix.rightToRight()
          << '\n';

std::cout << "Cross Delay = "
          << configuration.crossDelaySeconds * 1e6
          << " us\n";

std::cout << "Recursion Order = "
          << configuration.recursionOrder
          << "\n\n";

    RaceProcessor processor;

    processor.prepare(
        reader.sampleRate(),
        64);

    processor.configure(configuration);

    //------------------------------------------------------------
    // Samples verarbeiten
    //------------------------------------------------------------

    const auto& input = reader.samples();

    std::vector<StereoSample> output;
    output.reserve(input.size());

    for (const auto& sample : input)
    {
        float outLeft  = 0.0f;
        float outRight = 0.0f;

        processor.process(
            sample.left,
            sample.right,
            outLeft,
            outRight);

        output.push_back(
            {
                outLeft,
                outRight
            });
    }

    //------------------------------------------------------------
    // Ausgabe schreiben
    //------------------------------------------------------------

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
