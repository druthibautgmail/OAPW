#include "RACEDspEngine.h"
#include <iostream>
#include <algorithm>

RACEDspEngine::RACEDspEngine(float initialDn, float initialAttenuation, float initialCenterP, bool initialFreqLimit) {
    volume = 1.0f; 
    raceEnabled = true;
    filtersEnabled = true;

    updateFilters(44100.0);
    setParameters(initialDn, initialAttenuation, initialCenterP, initialFreqLimit);
}

RACEDspEngine::~RACEDspEngine() {}

void RACEDspEngine::updateFilters(double sampleRate) {
    // 1. Phasenlinearer Hochpass für den Bassschutz bei 150 Hz
    // Wir nutzen absichtlich nur EINEN Biquad (12dB/Okt), um die Phasendrehung
    // auf 180 Grad zu halbieren und Kammfilter-Effekte im Grundton zu minimieren.
    hpL1.setButterworth(Biquad::HIGHPASS, 150.0, sampleRate);
    hpR1.setButterworth(Biquad::HIGHPASS, 150.0, sampleRate);

    // 2. Akustisches Head-Shadowing Modell (High-Shelf)
    // Ab 2000 Hz dämpfen wir das Signal sanft um -12 dB ab, 
    // anstatt es mit einem harten Lowpass (Brickwall) abzuschneiden.
    hsL.setHighShelf(2000.0, sampleRate, -12.0);
    hsR.setHighShelf(2000.0, sampleRate, -12.0);
}

void RACEDspEngine::setParameters(float newDn, float newAttenuation, float newCenterP, bool newFreqLimit) {
    std::lock_guard<std::mutex> lock(dspMutex); 
    dn = (newDn > 0.0f) ? newDn : 1.0f;
    attenuation = newAttenuation;
    centerP = newCenterP;
    freqLimitRACE = newFreqLimit;

    delaySamples = dn;
    delayBufferL.assign(2205, 0.0f);
    delayBufferR.assign(2205, 0.0f);
    writeIndex = 0;
}

void RACEDspEngine::setVolume(float newVolume) { std::lock_guard<std::mutex> lock(dspMutex); volume = newVolume; }
void RACEDspEngine::setRaceEnabled(bool enabled) { std::lock_guard<std::mutex> lock(dspMutex); raceEnabled = enabled; }
void RACEDspEngine::setFiltersEnabled(bool enabled) { std::lock_guard<std::mutex> lock(dspMutex); filtersEnabled = enabled; }

float RACEDspEngine::getDn() { std::lock_guard<std::mutex> lock(dspMutex); return dn; }
float RACEDspEngine::getAttenuation() { std::lock_guard<std::mutex> lock(dspMutex); return attenuation; }
float RACEDspEngine::getCenterP() { std::lock_guard<std::mutex> lock(dspMutex); return centerP; }
bool RACEDspEngine::getFreqLimit() { std::lock_guard<std::mutex> lock(dspMutex); return freqLimitRACE; }
float RACEDspEngine::getVolume() { std::lock_guard<std::mutex> lock(dspMutex); return volume; }
bool RACEDspEngine::getRaceEnabled() { std::lock_guard<std::mutex> lock(dspMutex); return raceEnabled; }
bool RACEDspEngine::getFiltersEnabled() { std::lock_guard<std::mutex> lock(dspMutex); return filtersEnabled; }

inline float RACEDspEngine::applyBandpassL(float in) {
    float x = static_cast<float>(hpL1.process(in));
    x = static_cast<float>(hsL.process(x));
    return x;
}

inline float RACEDspEngine::applyBandpassR(float in) {
    float x = static_cast<float>(hpR1.process(in));
    x = static_cast<float>(hsR.process(x));
    return x;
}

// 4-Punkt Hermite Interpolation für perfekten Hochton-Erhalt
inline float RACEDspEngine::hermiteInterpolation(float fraction, float y0, float y1, float y2, float y3) {
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    return c0 + fraction * (c1 + fraction * (c2 + fraction * c3));
}

void RACEDspEngine::processSamples(std::vector<float>& interleavedSamples) {
    std::lock_guard<std::mutex> lock(dspMutex); 
    int bufSize = static_cast<int>(delayBufferL.size());
    if (bufSize == 0) return;

    for (size_t i = 0; i + 1 < interleavedSamples.size(); i += 2) {
        float currentL = interleavedSamples[i];
        float currentR = interleavedSamples[i + 1];

        if (!raceEnabled) {
            interleavedSamples[i] = currentL * volume;
            interleavedSamples[i + 1] = currentR * volume;
            continue;
        }

        delayBufferL[writeIndex] = currentL;
        delayBufferR[writeIndex] = currentR;

        // --- FRACTIONAL DELAY LOGIK ---
        int delayInt = static_cast<int>(delaySamples);
        float fraction = delaySamples - delayInt;

        // 4 Stützpunkte im Ringpuffer finden (mit bufSize-Addition gegen negative Indizes)
        int idx0 = (writeIndex - delayInt + 1 + bufSize) % bufSize;
        int idx1 = (writeIndex - delayInt + bufSize) % bufSize;
        int idx2 = (writeIndex - delayInt - 1 + bufSize) % bufSize;
        int idx3 = (writeIndex - delayInt - 2 + bufSize) % bufSize;

        // Sub-Sample genau berechnen
        float delayedL = hermiteInterpolation(fraction, delayBufferL[idx0], delayBufferL[idx1], delayBufferL[idx2], delayBufferL[idx3]);
        float delayedR = hermiteInterpolation(fraction, delayBufferR[idx0], delayBufferR[idx1], delayBufferR[idx2], delayBufferR[idx3]);
        // ------------------------------

        writeIndex = (writeIndex + 1) % bufSize;

        float crossTalkL = attenuation * (delayedR - centerP * delayedL) / (1.0f + centerP);
        float crossTalkR = attenuation * (delayedL - centerP * delayedR) / (1.0f + centerP);

        float diffL = -crossTalkL;
        float diffR = -crossTalkR;

        if (filtersEnabled && freqLimitRACE) {
            diffL = applyBandpassL(diffL);
            diffR = applyBandpassR(diffR);
        }

        float outL = (currentL + diffL) * volume;
        float outR = (currentR + diffR) * volume;

        if (outL < -1.0f) outL = -1.0f;
        if (outL > 1.0f)  outL = 1.0f;
        if (outR < -1.0f) outR = -1.0f;
        if (outR > 1.0f)  outR = 1.0f;

        interleavedSamples[i] = outL;
        interleavedSamples[i + 1] = outR;
    }
}
