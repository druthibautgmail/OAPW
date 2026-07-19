#include "oapw/io/wav_writer.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>

namespace oapw::io
{

namespace
{

struct ChunkHeader
{
    char id[4];
    std::uint32_t size;
};

}

bool WavWriter::write(
    const std::string& filename,
    const std::vector<StereoSample>& samples,
    std::uint32_t sampleRate)
{
    std::ofstream file(
        filename,
        std::ios::binary);

    if (!file)
    {
        return false;
    }

    constexpr std::uint16_t channels = 2;
    constexpr std::uint16_t bitsPerSample = 16;

    const std::uint16_t blockAlign =
        channels * bitsPerSample / 8;

    const std::uint32_t byteRate =
        sampleRate * blockAlign;

    const std::uint32_t dataSize =
        static_cast<std::uint32_t>(
            samples.size() * blockAlign);

    const std::uint32_t riffSize =
        36 + dataSize;

    //----------------------------------------------------------
    // RIFF Header
    //----------------------------------------------------------

    file.write("RIFF", 4);
    file.write(
        reinterpret_cast<const char*>(&riffSize),
        sizeof(riffSize));

    file.write("WAVE", 4);

    //----------------------------------------------------------
    // fmt chunk
    //----------------------------------------------------------

    ChunkHeader fmt =
    {
        {'f','m','t',' '},
        16
    };

    file.write(
        reinterpret_cast<const char*>(&fmt),
        sizeof(fmt));

    std::uint16_t audioFormat = 1;

    file.write(
        reinterpret_cast<const char*>(&audioFormat),
        sizeof(audioFormat));

    file.write(
        reinterpret_cast<const char*>(&channels),
        sizeof(channels));

    file.write(
        reinterpret_cast<const char*>(&sampleRate),
        sizeof(sampleRate));

    file.write(
        reinterpret_cast<const char*>(&byteRate),
        sizeof(byteRate));

    file.write(
        reinterpret_cast<const char*>(&blockAlign),
        sizeof(blockAlign));

    file.write(
        reinterpret_cast<const char*>(&bitsPerSample),
        sizeof(bitsPerSample));

    //----------------------------------------------------------
    // data chunk
    //----------------------------------------------------------

    ChunkHeader data =
    {
        {'d','a','t','a'},
        dataSize
    };

    file.write(
        reinterpret_cast<const char*>(&data),
        sizeof(data));

    for (const auto& sample : samples)
    {
        const float left =
            std::clamp(sample.left, -1.0f, 1.0f);

        const float right =
            std::clamp(sample.right, -1.0f, 1.0f);

        const std::int16_t left16 =
            static_cast<std::int16_t>(left * 32767.0f);

        const std::int16_t right16 =
            static_cast<std::int16_t>(right * 32767.0f);

        file.write(
            reinterpret_cast<const char*>(&left16),
            sizeof(left16));

        file.write(
            reinterpret_cast<const char*>(&right16),
            sizeof(right16));
    }

    return static_cast<bool>(file);
}

} // namespace oapw::io
