#ifndef RACE_DSP_ENGINE_H
#define RACE_DSP_ENGINE_H

#include <vector>
#include <mutex>
#include "Biquad.h"

class RACEDspEngine {
private:
    float attenuation;
    float centerP;
    float dn; // Interaurales Delay in Sub-Samples
    bool freqLimitRACE;
    float volume; 
    bool raceEnabled;
    bool filtersEnabled;

    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int writeIndex;
    float delaySamples;

    // OPTIMIERT: Nur noch ein Highpass (Phase) und ein High-Shelf (Head-Shadow) pro Kanal
    Biquad hpL1, hsL;
    Biquad hpR1, hsR;

    std::mutex dspMutex;

    void updateFilters(double sampleRate = 44100.0);
    inline float applyBandpassL(float in);
    inline float applyBandpassR(float in);

    // Hochpräzise 4-Punkt Hermite-Interpolation
    inline float hermiteInterpolation(float fraction, float y0, float y1, float y2, float y3);

public:
    RACEDspEngine(float initialDn, float initialAttenuation, float initialCenterP, bool initialFreqLimit);
    ~RACEDspEngine();

    void setParameters(float newDn, float newAttenuation, float newCenterP, bool newFreqLimit);
    void setVolume(float newVolume); 
    void setRaceEnabled(bool enabled);
    void setFiltersEnabled(bool enabled);

    float getDn(); 
    float getAttenuation();
    float getCenterP();
    bool getFreqLimit();
    float getVolume();
    bool getRaceEnabled();
    bool getFiltersEnabled();

    void processSamples(std::vector<float>& interleavedSamples);
};

#endif // RACE_DSP_ENGINE_H
