#include "RACEDspEngine.h"
#include <iostream>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- NEU: BiquadFilter Implementierung ---
BiquadFilter::BiquadFilter() : z1(0.0f), z2(0.0f), sampleRate(44100) {
    setParameters(1000.0f, 0.707f, 0.0f); 
}

void BiquadFilter::setParameters(float frequency, float qFactor, float gain, int sr) {
    freq = frequency; q = qFactor; gainDb = gain; sampleRate = sr;
    
    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * M_PI * freq / sampleRate;
    float alpha = std::sin(w0) / (2.0f * q);
    
    float a0 = 1.0f + alpha / A;
    b0 = (1.0f + alpha * A) / a0;
    b1 = (-2.0f * std::cos(w0)) / a0;
    b2 = (1.0f - alpha * A) / a0;
    a1 = (-2.0f * std::cos(w0)) / a0;
    a2 = (1.0f - alpha / A) / a0;
}

float BiquadFilter::process(float in) {
    float out = in * b0 + z1;
    z1 = in * b1 - out * a1 + z2;
    z2 = in * b2 - out * a2;
    return out;
}
// ------------------------------------------

IIRFilter::IIRFilter(const std::vector<double>& a, const std::vector<double>& b) 
    : ac(a), bc(b) {
    order = ac.size() - 1;
    x.resize(ac.size(), 0.0);
    y.resize(ac.size(), 0.0);
}

double IIRFilter::process(double input) {
    for (int n = order; n > 0; --n) {
        x[n] = x[n - 1];
        y[n] = y[n - 1];
    }
    x[0] = input;
    
    y[0] = ac[0] * x[0];
    for (int n = 1; n <= order; ++n) {
        y[0] += (ac[n] * x[n] - bc[n] * y[n]);
    }
    return y[0];
}

const std::vector<double> AC_LP = { 1.707930066E-11, 3.245067125E-10, 2.92056041249E-9, 1.654984233742E-8, 6.619936934968E-8, 1.9859810804904E-7, 4.6339558544777E-7, 8.6059180154585E-7, 1.29088770231878E-6, 1.5777516361674E-6, 1.5777516361674E-6, 1.29088770231878E-6, 8.6059180154585E-7, 4.6339558544777E-7, 1.9859810804904E-7, 6.619936934968E-8, 1.654984233742E-8, 2.92056041249E-9, 3.245067125E-10, 1.707930066E-11 };
const std::vector<double> BC_LP = { 1.0, -11.23829931452124, 60.88662121602015, -211.01791497183748, 523.7659541722651, -988.1021170488126, 1467.8446929796328, -1755.5644457376006, 1714.2251020332717, -1377.7264003960127, 914.6210265583827, -501.2759048988774, 225.7748760019649, -82.8018677820916, 24.357062095258804, -5.613879220355998, 0.9772797930828155, -0.12090187565166291, 0.009478452885452278, -3.541797659591719E-4 };
const std::vector<double> AC_HP = { 0.018501938638030006, -0.35153683412257014, 3.163831507103131, -17.928378540251078, 71.71351416100431, -215.14054248301292, 501.99459912703014, -932.2756840930559, 1398.413526139584, -1709.172087503936, 1709.172087503936, -1398.413526139584, 932.2756840930559, -501.99459912703014, 215.14054248301292, -71.71351416100431, 17.928378540251078, -3.163831507103131, 0.35153683412257014, -0.018501938638030006 };
const std::vector<double> BC_HP = { 1.0, -11.23829931456545, 60.8866212164491, -211.01791497385992, 523.7659541784101, -988.102117062272, 1467.8446930021598, -1755.5644457674098, 1714.2251020651054, -1377.7264004237784, 914.6210265782755, -501.27590491059107, 225.77487600760983, -82.80186778429722, 24.35706209594665, -5.613879220523086, 0.9772797931132484, -0.1209018756555653, 0.009478452885765523, -3.54179765970956E-4 };

RACEDspEngine::RACEDspEngine(float initialDelayUs, float initialAttenuationDb, float initialCenterP, bool initialFreqLimit) 
    : lpfL(AC_LP, BC_LP), lpfR(AC_LP, BC_LP),
      hpfL(AC_HP, BC_HP), hpfR(AC_HP, BC_HP) {
    
    volume = 1.0f; 
    raceEnabled = true;
    filtersEnabled = true;
    eqEnabled = false; // EQ startet standardmäßig aus

    // EQ Initialisierung: Low, Mid, High
    for (int i = 0; i < 3; ++i) {
        float f = (i == 0) ? 100.0f : (i == 1) ? 1000.0f : 5000.0f;
        eqL[i].setParameters(f, 0.707f, 0.0f);
        eqR[i].setParameters(f, 0.707f, 0.0f);
    }

    setParameters(initialDelayUs, initialAttenuationDb, initialCenterP, initialFreqLimit);
}

RACEDspEngine::~RACEDspEngine() {}

void RACEDspEngine::setParameters(float newDelayUs, float newAttenuationDb, float newCenterP, bool newFreqLimit) {
    std::lock_guard<std::mutex> lock(dspMutex); 
    
    delayUs = newDelayUs;
    attenuationDb = newAttenuationDb;
    centerP = newCenterP;
    freqLimitRACE = newFreqLimit;

    int calculatedDn = static_cast<int>(std::round(delayUs * 44100.0f / 1000000.0f));
    dn = (calculatedDn > 0) ? calculatedDn : 1;
    attenuationLinear = std::pow(10.0f, attenuationDb / 20.0f);

    delayBufferL.resize(dn, 0.0f);
    delayBufferR.resize(dn, 0.0f);
    writeIndex = 0;
}

void RACEDspEngine::setVolume(float newVolume) { std::lock_guard<std::mutex> lock(dspMutex); volume = newVolume; }
void RACEDspEngine::setRaceEnabled(bool enabled) { std::lock_guard<std::mutex> lock(dspMutex); raceEnabled = enabled; }
void RACEDspEngine::setFiltersEnabled(bool enabled) { std::lock_guard<std::mutex> lock(dspMutex); filtersEnabled = enabled; }

// NEU: EQ Methoden
void RACEDspEngine::setEqEnabled(bool enabled) { std::lock_guard<std::mutex> lock(dspMutex); eqEnabled = enabled; }
void RACEDspEngine::setEqBand(int b, float f, float q, float g) {
    std::lock_guard<std::mutex> lock(dspMutex);
    if(b >= 0 && b < 3) {
        eqL[b].setParameters(f, q, g);
        eqR[b].setParameters(f, q, g);
    }
}

float RACEDspEngine::getDelayUs() { std::lock_guard<std::mutex> lock(dspMutex); return delayUs; }
float RACEDspEngine::getAttenuationDb() { std::lock_guard<std::mutex> lock(dspMutex); return attenuationDb; }
float RACEDspEngine::getCenterP() { std::lock_guard<std::mutex> lock(dspMutex); return centerP; }
bool RACEDspEngine::getFreqLimit() { std::lock_guard<std::mutex> lock(dspMutex); return freqLimitRACE; }
float RACEDspEngine::getVolume() { std::lock_guard<std::mutex> lock(dspMutex); return volume; }
bool RACEDspEngine::getRaceEnabled() { std::lock_guard<std::mutex> lock(dspMutex); return raceEnabled; }
bool RACEDspEngine::getFiltersEnabled() { std::lock_guard<std::mutex> lock(dspMutex); return filtersEnabled; }

bool RACEDspEngine::getEqEnabled() { std::lock_guard<std::mutex> lock(dspMutex); return eqEnabled; }
float RACEDspEngine::getEqFreq(int b) { std::lock_guard<std::mutex> lock(dspMutex); return (b>=0 && b<3) ? eqL[b].getFreq() : 0.0f; }
float RACEDspEngine::getEqQ(int b) { std::lock_guard<std::mutex> lock(dspMutex); return (b>=0 && b<3) ? eqL[b].getQ() : 0.0f; }
float RACEDspEngine::getEqGain(int b) { std::lock_guard<std::mutex> lock(dspMutex); return (b>=0 && b<3) ? eqL[b].getGain() : 0.0f; }


void RACEDspEngine::processSamples(std::vector<float>& interleavedSamples) {
    std::lock_guard<std::mutex> lock(dspMutex); 

    for (size_t i = 0; i + 1 < interleavedSamples.size(); i += 2) {
        float currentL = interleavedSamples[i];
        float currentR = interleavedSamples[i + 1];

        // Bypass komplett, falls RACE aus
        if (!raceEnabled) {
            interleavedSamples[i] = currentL * volume;
            interleavedSamples[i + 1] = currentR * volume;
            continue;
        }

        float lpL, lpR, hpL = 0.0f, hpR = 0.0f;

        if (filtersEnabled && freqLimitRACE) {
            hpL = static_cast<float>(hpfL.process(currentL));
            hpR = static_cast<float>(hpfR.process(currentR));
            lpL = static_cast<float>(lpfL.process(currentL));
            lpR = static_cast<float>(lpfR.process(currentR));
        } else {
            lpL = currentL;
            lpR = currentR;
        }

        float delayedL = delayBufferL[writeIndex];
        float delayedR = delayBufferR[writeIndex];

        float crossTalkL = attenuationLinear * (delayedR - centerP * delayedL) / (1.0f + centerP);
        float crossTalkR = attenuationLinear * (delayedL - centerP * delayedR) / (1.0f + centerP);

        delayBufferL[writeIndex] = lpL - crossTalkL;
        delayBufferR[writeIndex] = lpR - crossTalkR;

        writeIndex++;
        if (writeIndex >= dn) writeIndex = 0;

        // Rohes Ausgangssignal vor der Lautstaerkeregelung
        float rawOutL = delayedL + hpL;
        float rawOutR = delayedR + hpR;

        // NEU: PEQ anwenden, falls aktiv
        if (eqEnabled) {
            for (int b = 0; b < 3; ++b) {
                rawOutL = eqL[b].process(rawOutL);
                rawOutR = eqR[b].process(rawOutR);
            }
        }

        float outL = rawOutL * volume;
        float outR = rawOutR * volume;

        if (outL < -1.0f) outL = -1.0f;
        if (outL > 1.0f)  outL = 1.0f;
        if (outR < -1.0f) outR = -1.0f;
        if (outR > 1.0f)  outR = 1.0f;

        interleavedSamples[i] = outL;
        interleavedSamples[i + 1] = outR;
    }
}
