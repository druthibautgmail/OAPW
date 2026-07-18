#include "oapw/io/wav_reader.hpp"

#include <cstdint>
#include <cstring>
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

} // namespace

bool WavReader::open(const std::string& filename)
{
    samples_.clear();

    sampleRate_ = 0;
    bitsPerSample_ = 0;
    channelCount_ = 0;

    std::ifstream file(
        filename,
        std::ios::binary);

    if (!file)
    {
        return false;
    }

    char riff[4];
    std::uint32_t fileSize = 0;
    char wave[4];

    file.read(riff, sizeof(riff));
    file.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
    file.read(wave, sizeof(wave));

    if (!file)
    {
        return false;
    }

    if (std::memcmp(riff, "RIFF", 4) != 0)
    {
        return false;
    }

    if (std::memcmp(wave, "WAVE", 4) != 0)
    {
        return false;
    }

    bool fmtFound = false;
    bool dataFound = false;

    while (file)
    {
        ChunkHeader chunk{};

        file.read(
            reinterpret_cast<char*>(&chunk),
            sizeof(chunk));

        if (!file)
        {
            break;
        }

        //------------------------------------------------------------
        // fmt
        //------------------------------------------------------------

        if (std::memcmp(chunk.id, "fmt ", 4) == 0)
        {
            std::uint16_t audioFormat;
            std::uint16_t channels;
            std::uint32_t sampleRate;
            std::uint32_t byteRate;
            std::uint16_t blockAlign;
            std::uint16_t bitsPerSample;

            file.read(
                reinterpret_cast<char*>(&audioFormat),
                sizeof(audioFormat));

            file.read(
                reinterpret_cast<char*>(&channels),
                sizeof(channels));

            file.read(
                reinterpret_cast<char*>(&sampleRate),
                sizeof(sampleRate));

            file.read(
                reinterpret_cast<char*>(&byteRate),
                sizeof(byteRate));

            file.read(
                reinterpret_cast<char*>(&blockAlign),
                sizeof(blockAlign));

            file.read(
                reinterpret_cast<char*>(&bitsPerSample),
                sizeof(bitsPerSample));

            if (!file)
            {
                return false;
            }

            if (chunk.size > 16)
            {
                file.seekg(
                    chunk.size - 16,
                    std::ios::cur);
            }

            if (audioFormat != 1)
            {
                return false;
            }

            if (channels != 2)
            {
                return false;
            }

            if (sampleRate != 44100)
            {
                return false;
            }

            if (bitsPerSample != 16)
            {
                return false;
            }

            sampleRate_ = sampleRate;
            channelCount_ = channels;
            bitsPerSample_ = bitsPerSample;

            fmtFound = true;
        }

        //------------------------------------------------------------
        // data
        //------------------------------------------------------------

        else if (std::memcmp(chunk.id, "data", 4) == 0)
        {
            if (!fmtFound)
            {
                return false;
            }

            dataFound = true;

            const std::size_t frameCount =
                chunk.size / (2 * sizeof(std::int16_t));

            samples_.reserve(frameCount);

            constexpr float scale =
                1.0f / 32768.0f;

            for (std::size_t i = 0; i < frameCount; ++i)
            {
                std::int16_t left;
                std::int16_t right;

                file.read(
                    reinterpret_cast<char*>(&left),
                    sizeof(left));

                file.read(
                    reinterpret_cast<char*>(&right),
                    sizeof(right));

                if (!file)
                {
                    return false;
                }

                samples_.push_back(
                    {
                        static_cast<float>(left) * scale,
                        static_cast<float>(right) * scale
                    });
            }

            break;
        }

        //------------------------------------------------------------
        // alle anderen Chunks überspringen
        //------------------------------------------------------------

        else
        {
            file.seekg(
                chunk.size,
                std::ios::cur);
        }

        // RIFF-Chunks sind wortausgerichtet
        if (chunk.size & 1U)
        {
            file.seekg(1, std::ios::cur);
        }
    }

    return fmtFound && dataFound;
}

std::size_t WavReader::sampleCount() const
{
    return samples_.size();
}

const std::vector<StereoSample>& WavReader::samples() const
{
    return samples_;
}

std::uint32_t WavReader::sampleRate() const
{
    return sampleRate_;
}

std::uint16_t WavReader::bitsPerSample() const
{
    return bitsPerSample_;
}

std::uint16_t WavReader::channelCount() const
{
    return channelCount_;
}

} // namespace oapw::io
