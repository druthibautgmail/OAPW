#include "oapw/io/wav_reader.hpp"
#include "oapw/io/wav_writer.hpp"
#include "oapw/core/race_processor.hpp" // Neu hinzugefügt
#include "oapw/config/race_configuration.hpp" // Neu hinzugefügt

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: wav_reader_demo <file.wav>\n";
        return 1;
    }

    oapw::io::WavReader reader;

    if (!reader.open(argv[1]))
    {
        std::cerr << "Unable to read WAV file.\n";
        return 1;
    }

    std::cout << "Sample rate : " << reader.sampleRate() << " Hz\n";
    std::cout << "Channels    : " << reader.channelCount() << '\n';
    std::cout << "Bits/sample : " << reader.bitsPerSample() << '\n';
    std::cout << "Frames      : " << reader.sampleCount() << "\n\n";

    const auto& inputSamples = reader.samples();

    //------------------------------------------------------------------
    // RACE Prozessor Setup
    //------------------------------------------------------------------
    oapw::core::RaceProcessor processor;
    
    // Maximale Verzögerung großzügig wählen (z.B. 64 Samples)
    processor.prepare(reader.sampleRate(), 64);

    // Standard-Ambiophonie-Dipol-Setup konfigurieren:
    // Für ein typisches physikalisches Setup stehen die Lautsprecher z.B. bei +- 10 Grad.
    // Das entspricht einem Crosstalk-Delay von ca. 4-6 Samples bei 48kHz.
    // Wir setzen hier testweise ein Delay von ca. 0.0001 Sekunden (ca. 4.8 Samples bei 48kHz).
    oapw::config::RaceConfiguration config;
    config.crossDelaySeconds = 0.0001; 
    
    // GainMatrix für das Übersprechen (Lautsprecher zum fernen Ohr)
    // Ein Dämpfungsfaktor g von 0.7 bis 0.85 ist typisch für die Rekursion
    config.gainMatrix.setGains(
        1.0f,   // Left to Left (Direktsignal)
        1.0f,   // Right to Right (Direktsignal)
        0.75f,  // Left to Right (Crosstalk Dämpfung)
        0.75f   // Right to Left (Crosstalk Dämpfung)
    );

    processor.configure(config);

    //------------------------------------------------------------------
    // Verarbeitungsschleife
    //------------------------------------------------------------------
    std::cout << "Processing audio with Glasgal RACE algorithm...\n";
    
    // Vektor für die bearbeiteten Ausgangssamples vorbereiten
    auto processedSamples = inputSamples; 

    for (std::size_t i = 0; i < inputSamples.size(); ++i)
    {
        // Sample-weise durch den RACE-Feedbackprozessor jagen
        processor.process(
            inputSamples[i].left,
            inputSamples[i].right,
            processedSamples[i].left,
            processedSamples[i].right
        );
    }

    //------------------------------------------------------------------
    // Write copy (Original-Kopie)
    //------------------------------------------------------------------
    oapw::io::WavWriter writer;
    if (!writer.write("copy.wav", inputSamples, reader.sampleRate()))
    {
        std::cerr << "\nUnable to write copy.wav\n";
        return 1;
    }
    std::cout << "Successfully wrote copy.wav (Original Reference)\n";

    //------------------------------------------------------------------
    // Write processed (Ambiophonisches Signal)
    //------------------------------------------------------------------
    if (!writer.write("processed.wav", processedSamples, reader.sampleRate()))
    {
        std::cerr << "Unable to write processed.wav\n";
        return 1;
    }
    std::cout << "Successfully wrote processed.wav (Ambiophonic Output)\n\n";

    std::cout << "Done. You can now compare copy.wav and processed.wav!\n";
    return 0;
}
