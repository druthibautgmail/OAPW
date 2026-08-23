#ifndef RACE_DSP_ENGINE_H
#define RACE_DSP_ENGINE_H

#include <vector>
#include <mutex>
#include <array>

// NEU: Die Biquad-Filter Klasse für den Equalizer
class BiquadFilter {
private:
    float b0, b1, b2, a1, a2;
    float z1, z2; 
    float freq, q, gainDb;
    int sampleRate;

public:
    BiquadFilter();
    void setParameters(float frequency, float qFactor, float gainDb, int sampleRate = 44100);
    float process(float in);
    
    float getFreq() const { return freq; }
    float getQ() const { return q; }
    float getGain() const { return gainDb; }
};

class IIRFilter {
private:
    std::vector<double> ac;
    std::vector<double> bc;
    std::vector<double> x;
    std::vector<double> y;
    int order;

public:
    IIRFilter(const std::vector<double>& a, const std::vector<double>& b);
    double process(double input);
};

class RACEDspEngine {
private:
    float delayUs;           
    float attenuationDb;     
    float attenuationLinear; 
    int dn;                  
    
    float centerP;
    bool freqLimitRACE;
    float volume; 
    bool raceEnabled;
    bool filtersEnabled;
    bool eqEnabled; // NEU: Globaler EQ Schalter

    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int writeIndex;

    IIRFilter lpfL;
    IIRFilter lpfR;
    IIRFilter hpfL;
    IIRFilter hpfR;

    // NEU: 3 EQ-Baender pro Kanal
    std::array<BiquadFilter, 3> eqL;
    std::array<BiquadFilter, 3> eqR;

    std::mutex dspMutex;

public:
    RACEDspEngine(float initialDelayUs, float initialAttenuationDb, float initialCenterP, bool initialFreqLimit);
    ~RACEDspEngine();

    void setParameters(float newDelayUs, float newAttenuationDb, float newCenterP, bool newFreqLimit);
    void setVolume(float newVolume); 
    void setRaceEnabled(bool enabled);
    void setFiltersEnabled(bool enabled);
    
    // NEU: EQ-Steuerung
    void setEqEnabled(bool enabled);
    void setEqBand(int bandIndex, float freq, float q, float gainDb);

    float getDelayUs();
    float getAttenuationDb();
    float getCenterP();
    bool getFreqLimit();
    float getVolume();
    bool getRaceEnabled();
    bool getFiltersEnabled();
    
    // NEU: EQ-Getter
    bool getEqEnabled();
    float getEqFreq(int bandIndex);
    float getEqQ(int bandIndex);
    float getEqGain(int bandIndex);

    void processSamples(std::vector<float>& interleavedSamples);
};

#endif // RACE_DSP_ENGINE_H
