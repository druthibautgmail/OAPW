#include "RACEDspEngine.h"
#include <iostream>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- NEU: Eigener, extrem schneller FFT Algorithmus ---
void computeFFT(std::vector<std::complex<float>>& data) {
    size_t n = data.size();
    if (n <= 1) return;
    
    // Bit-Reversal
    for (size_t i = 1, j = 0; i < n; i++) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(data[i], data[j]);
    }
    
    // Cooley-Tukey
    for (size_t len = 2; len <= n; len <<= 1) {
        float angle = -2.0f * M_PI / len;
        std::complex<float> wlen(std::cos(angle), std::sin(angle));
        for (size_t i = 0; i < n; i += len) {
            std::complex<float> w(1, 0);
            for (size_t j = 0; j < len / 2; j++) {
                std::complex<float> u = data[i + j];
                std::complex<float> v = data[i + j + len / 2] * w;
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}
// ------------------------------------------------------

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

    // ... nach der EQ Initialisierung eingefügt:
    spectrumBuffer.resize(1024, 0.0f);
    spectrumIndex = 0;

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

        // Bypass komplett, aber Frequenzbandanzeige weiterhin eingeschaltet, falls RACE ausgeschaltet ist!
        if (!raceEnabled) {
            float bypassL = currentL * volume;
            float bypassR = currentR * volume;
            
            interleavedSamples[i] = bypassL;
            interleavedSamples[i + 1] = bypassR;
            
            // NEU: Signal auch im reinen Bypass-Modus für den Analyzer abgreifen
            spectrumBuffer[spectrumIndex] = (bypassL + bypassR) * 0.5f;
            spectrumIndex = (spectrumIndex + 1) % 1024;
            
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

        // ... (Bisheriger Code in processSamples)
        
        if (outL < -1.0f) outL = -1.0f;
        if (outL > 1.0f)  outL = 1.0f;
        if (outR < -1.0f) outR = -1.0f;
        if (outR > 1.0f)  outR = 1.0f;

// NEU: Signal (Mono-Mix) für den Analyzer abgreifen
        spectrumBuffer[spectrumIndex] = (outL + outR) * 0.5f;
        spectrumIndex = (spectrumIndex + 1) % 1024;

        interleavedSamples[i] = outL;
        interleavedSamples[i + 1] = outR;
    }
} // <--- HIER schließt sich processSamples() korrekt!

std::vector<float> RACEDspEngine::getSpectrumBands() {
    std::vector<float> audioData(1024, 0.0f);
    
    // 1. Sichere Kopie der aktuellen Audiodaten ziehen
    {
        std::lock_guard<std::mutex> lock(dspMutex);
        for(int i = 0; i < 1024; i++) {
            audioData[i] = spectrumBuffer[(spectrumIndex + i) % 1024];
        }
    } // Mutex hier sofort freigeben, damit das Audio nicht beim Rechnen stockt!

    // 2. Hann-Fenster anwenden & in komplexe Zahlen wandeln
    std::vector<std::complex<float>> complexData(1024);
    for(int i = 0; i < 1024; i++) {
        float multiplier = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / 1023.0f));
        complexData[i] = std::complex<float>(audioData[i] * multiplier, 0.0f);
    }

    // 3. FFT berechnen
    computeFFT(complexData);

    // 4. In 64 logarithmische Anzeige-Balken umrechnen (20Hz bis 20kHz)
    std::vector<float> bands(64, 0.0f);
    float minFreq = 20.0f;
    float maxFreq = 22050.0f;
    float logMin = std::log10(minFreq);
    float logMax = std::log10(maxFreq);

    for (int i = 1; i < 512; i++) {
        float freq = (float)i * 44100.0f / 1024.0f;
        if (freq < minFreq) continue;
        if (freq > maxFreq) break;
        
        int bandIndex = (int)((std::log10(freq) - logMin) / (logMax - logMin) * 64.0f);
        if (bandIndex >= 0 && bandIndex < 64) {
            float mag = std::abs(complexData[i]);
            if (mag > bands[bandIndex]) bands[bandIndex] = mag; // Peak im Band merken
        }
    }

    // 5. In Dezibel umrechnen und für das HTML Canvas normalisieren (0.0 bis 1.0)
    for(int i = 0; i < 64; i++) {
        float db = 20.0f * std::log10(bands[i] + 1e-6f);
        float normalized = (db + 70.0f) / 70.0f; // -70dB bis 0dB Skala
        if (normalized < 0.0f) normalized = 0.0f;
        if (normalized > 1.0f) normalized = 1.0f;
        bands[i] = normalized;
    }

    return bands;
}
