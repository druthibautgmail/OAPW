#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "httplib.h"

#include "RACEDspEngine.h"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <deque>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// ---------------------------------------------------------
// Das HTML Frontend für den Browser (Dark Mode)
// ---------------------------------------------------------
const char* HTML_CONTENT = R"HTML(
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>OAPW Control</title>
    <style>
        body { background-color: #121212; color: #ffffff; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; padding: 2rem; max-width: 600px; margin: 0 auto; }
        h1 { font-weight: 300; border-bottom: 1px solid #333; padding-bottom: 10px; margin-bottom: 2rem; }
        .control-group { margin-bottom: 1.5rem; background: #1e1e1e; padding: 1.5rem; border-radius: 8px; }
        label { display: flex; justify-content: space-between; margin-bottom: 0.5rem; font-weight: bold; }
        input[type="range"] { width: 100%; cursor: pointer; }
        .value-display { font-weight: normal; color: #aaa; }
        button { width: 100%; padding: 15px; font-size: 1.2rem; font-weight: bold; border: none; border-radius: 8px; cursor: pointer; background: #007aff; color: white; margin-bottom: 1.0rem; transition: background 0.3s;}
        button.off { background: #333; color: #888; }
    </style>
</head>
<body>
    <h1>Spa OAPW Ambiophonics</h1>
    
    <button id="btnRace" onclick="toggleRace()">RACE: AN</button>
    <button id="btnFilters" onclick="toggleFilters()">Filter: AN</button>

    <div class="control-group">
        <label>Lautstärke <span id="valVol" class="value-display">50%</span></label>
        <input type="range" id="vol" min="0" max="100" value="50" oninput="updateParam('vol', this.value)">
    </div>

    <div class="control-group">
        <label>Interaural Delay (dn) <span id="valDn" class="value-display">11</span></label>
        <input type="range" id="dn" min="1" max="50" value="11" oninput="updateParam('dn', this.value)">
    </div>

    <div class="control-group">
        <label>Attenuation <span id="valAtt" class="value-display">0.50</span></label>
        <input type="range" id="att" min="0" max="100" value="50" oninput="updateParam('att', this.value)">
    </div>

    <div class="control-group">
        <label>Center Parameter <span id="valCenter" class="value-display">0.10</span></label>
        <input type="range" id="center" min="0" max="100" value="10" oninput="updateParam('center', this.value)">
    </div>

    <script>
        let raceActive = true;
        let filtersActive = true;

        function updateParam(param, value) {
            if(param === 'vol') document.getElementById('valVol').innerText = value + '%';
            if(param === 'dn') document.getElementById('valDn').innerText = value;
            if(param === 'att') document.getElementById('valAtt').innerText = (value/100).toFixed(2);
            if(param === 'center') document.getElementById('valCenter').innerText = (value/100).toFixed(2);

            fetch(`/api/update?${param}=${value}`);
        }

        function toggleRace() {
            raceActive = !raceActive;
            let btn = document.getElementById('btnRace');
            btn.innerText = raceActive ? "RACE: AN" : "RACE: AUS";
            btn.className = raceActive ? "" : "off";
            fetch(`/api/update?race=${raceActive ? 1 : 0}`);
        }

        function toggleFilters() {
            filtersActive = !filtersActive;
            let btn = document.getElementById('btnFilters');
            btn.innerText = filtersActive ? "Filter: AN" : "Filter: AUS";
            btn.className = filtersActive ? "" : "off";
            fetch(`/api/update?filters=${filtersActive ? 1 : 0}`);
        }

        window.onload = () => {
            fetch('/api/status').then(r => r.json()).then(data => {
                document.getElementById('vol').value = data.volume * 100;
                document.getElementById('valVol').innerText = Math.round(data.volume * 100) + '%';
                
                document.getElementById('dn').value = data.dn;
                document.getElementById('valDn').innerText = data.dn;
                
                document.getElementById('att').value = data.attenuation * 100;
                document.getElementById('valAtt').innerText = data.attenuation.toFixed(2);
                
                document.getElementById('center').value = data.centerP * 100;
                document.getElementById('valCenter').innerText = data.centerP.toFixed(2);

                raceActive = data.raceActive;
                let btn = document.getElementById('btnRace');
                btn.innerText = raceActive ? "RACE: AN" : "RACE: AUS";
                btn.className = raceActive ? "" : "off";

                filtersActive = data.filtersActive;
                let btnF = document.getElementById('btnFilters');
                btnF.innerText = filtersActive ? "Filter: AN" : "Filter: AUS";
                btnF.className = filtersActive ? "" : "off";
            });
        };
    </script>
</body>
</html>
)HTML";

// ---------------------------------------------------------
// Helferfunktionen & FIFO Puffer
// ---------------------------------------------------------
int getKeyboardInput() {
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    return (ch != EOF) ? ch : -1;
}

void printStatus(float vol, int dn, float att, float center, bool raceActive, bool filtersActive) {
    std::cout << "\r[Status] Vol: " << (int)(vol * 100) << "% | "
              << "RACE: " << (raceActive ? "AN" : "AUS") << " | "
              << "Filter: " << (filtersActive ? "AN" : "AUS") << " | "
              << "Delay(dn): " << dn << " | "
              << "Att: " << att << " | "
              << "Center: " << center << "      " << std::flush;
}

class ThreadSafeAudioBuffer {
private:
    std::deque<float> buffer;
    std::mutex mtx;
public:
    void push(const std::vector<float>& data) {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.insert(buffer.end(), data.begin(), data.end());
    }
    size_t pull(float* outData, size_t numSamples) {
        std::lock_guard<std::mutex> lock(mtx);
        size_t available = buffer.size();
        size_t toRead = (available < numSamples) ? available : numSamples;
        for(size_t i = 0; i < toRead; ++i) outData[i] = buffer[i];
        buffer.erase(buffer.begin(), buffer.begin() + toRead);
        return toRead;
    }
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return buffer.size();
    }
};

struct AudioContext {
    ThreadSafeAudioBuffer* sharedBuffer;
};

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioContext* pContext = (AudioContext*)pDevice->pUserData;
    if (pContext == nullptr) return;
    float* pOutputF32 = (float*)pOutput;
    size_t samplesRequested = frameCount * pDevice->playback.channels;
    size_t samplesRead = pContext->sharedBuffer->pull(pOutputF32, samplesRequested);
    for (size_t i = samplesRead; i < samplesRequested; ++i) pOutputF32[i] = 0.0f;
}

// ---------------------------------------------------------
// Hintergrund-Thread: Lesen & DSP-Berechnung
// ---------------------------------------------------------
void audioProcessingLoop(RACEDspEngine* dspEngine, std::string mode, std::string filePath, ThreadSafeAudioBuffer* sharedBuffer, std::atomic<bool>& isRunning) {
    const size_t maxBufferSize = 44100 * 2 * 2; 

    if (mode == "stream") {
        const char* pipeName = "/tmp/oapw_stream";
        mkfifo(pipeName, 0666); 
        chmod(pipeName, 0666);
        
        int fd = -1; // KORREKTUR 1: fd wird initial auf -1 gesetzt
        std::vector<int16_t> intBuf(1024 * 2);
        std::vector<float> floatBuf(1024 * 2);

        while (isRunning) {
            // KORREKTUR 1: Robustes Neu-Öffnen der Pipe, falls sie noch nicht bereit ist
            if (fd < 0) {
                fd = open(pipeName, O_RDONLY);
                if (fd < 0) {
                    usleep(100000); // 100ms warten und erneut versuchen
                    continue; 
                }
            }

            if (sharedBuffer->size() > maxBufferSize) { usleep(10000); continue; }
            
            ssize_t bytesRead = read(fd, intBuf.data(), intBuf.size() * sizeof(int16_t));
            
            if (bytesRead > 0) {
                size_t samplesRead = bytesRead / sizeof(int16_t);
                
                // KORREKTUR 2: Wir erzwingen eine GERADE Anzahl an Samples (Links/Rechts Paare)
                size_t validSamples = (samplesRead / 2) * 2; 
                
                if (validSamples > 0) {
                    for (size_t i = 0; i < validSamples; ++i) {
                        floatBuf[i] = intBuf[i] / 32768.0f; 
                    }
                    std::vector<float> chunk(floatBuf.begin(), floatBuf.begin() + validSamples);
                    dspEngine->processSamples(chunk);
                    sharedBuffer->push(chunk);
                }
            } else if (bytesRead == 0) {
                // Writer hat die Pipe geschlossen -> fd zurücksetzen, damit neu geöffnet wird
                close(fd);
                fd = -1; 
            } else {
                usleep(10000); 
            }
        }
        if (fd >= 0) close(fd);
    } else if (mode == "file") {
        unsigned int channels, sampleRate;
        drwav_uint64 totalPCMFrameCount;
        float* sampleData = nullptr;

        if (filePath.find(".mp3") != std::string::npos) {
            drmp3_config config;
            sampleData = drmp3_open_file_and_read_pcm_frames_f32(filePath.c_str(), &config, &totalPCMFrameCount, NULL);
        } else {
            sampleData = drwav_open_file_and_read_pcm_frames_f32(filePath.c_str(), &channels, &sampleRate, &totalPCMFrameCount, NULL);
        }

        if (sampleData == nullptr) {
            std::cerr << "\nFehler: Konnte Datei nicht oeffnen." << std::endl;
            isRunning = false; return;
        }

        size_t totalSamples = totalPCMFrameCount * 2;
        size_t currentSample = 0;

        while (isRunning && currentSample < totalSamples) {
            if (sharedBuffer->size() > maxBufferSize) { usleep(10000); continue; }
            size_t samplesRemaining = totalSamples - currentSample;
            size_t samplesToProcess = (samplesRemaining < 2048) ? samplesRemaining : 2048;
            std::vector<float> chunk(sampleData + currentSample, sampleData + currentSample + samplesToProcess);
            dspEngine->processSamples(chunk);
            sharedBuffer->push(chunk);
            currentSample += samplesToProcess;
        }
        
        if (filePath.find(".mp3") != std::string::npos) drmp3_free(sampleData, NULL);
        else drwav_free(sampleData, NULL);
        
        while (isRunning && sharedBuffer->size() > 0) usleep(50000);
        isRunning = false; 
    }
}

// ---------------------------------------------------------
// Webserver-Thread (Lokale HTTP-API)
// ---------------------------------------------------------
void webServerLoop(RACEDspEngine* dspEngine, std::atomic<bool>& isRunning) {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(HTML_CONTENT, "text/html");
    });

    svr.Get("/api/status", [dspEngine](const httplib::Request&, httplib::Response& res) {
        std::string json = "{";
        json += "\"volume\":" + std::to_string(dspEngine->getVolume()) + ",";
        json += "\"dn\":" + std::to_string(dspEngine->getDn()) + ",";
        json += "\"attenuation\":" + std::to_string(dspEngine->getAttenuation()) + ",";
        json += "\"centerP\":" + std::to_string(dspEngine->getCenterP()) + ",";
        json += "\"raceActive\":";
        json += (dspEngine->getRaceEnabled() ? "true" : "false");
        json += ",\"filtersActive\":";
        json += (dspEngine->getFiltersEnabled() ? "true" : "false");
        json += "}";
        res.set_content(json, "application/json");
    });

    svr.Get("/api/update", [dspEngine](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("vol")) dspEngine->setVolume(std::stof(req.get_param_value("vol")) / 100.0f);
        if (req.has_param("race")) dspEngine->setRaceEnabled(std::stoi(req.get_param_value("race")) != 0);
        if (req.has_param("filters")) dspEngine->setFiltersEnabled(std::stoi(req.get_param_value("filters")) != 0);

        if (req.has_param("dn") || req.has_param("att") || req.has_param("center")) {
            int dn = dspEngine->getDn();
            float att = dspEngine->getAttenuation();
            float center = dspEngine->getCenterP();
            bool freqLimit = dspEngine->getFreqLimit();

            if (req.has_param("dn")) dn = std::stoi(req.get_param_value("dn"));
            if (req.has_param("att")) att = std::stof(req.get_param_value("att")) / 100.0f;
            if (req.has_param("center")) center = std::stof(req.get_param_value("center")) / 100.0f;

            dspEngine->setParameters(dn, att, center, freqLimit);
        }
        res.set_content("OK", "text/plain");
    });

    std::cout << "\n[Web] Web-Interface gestartet auf Port 8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
}

// ---------------------------------------------------------
// Hauptprogramm
// ---------------------------------------------------------
int main(int argc, char** argv) {
    std::string mode = "stream";
    std::string filePath = "";
    if (argc == 2) { mode = "file"; filePath = argv[1]; }

    std::atomic<bool> isRunning(true);
    RACEDspEngine dspEngine(11, 0.5f, 0.1f, true);
    dspEngine.setVolume(0.5f);

    ThreadSafeAudioBuffer sharedBuffer;
    AudioContext context;
    context.sharedBuffer = &sharedBuffer;

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32; 
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 44100;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &context;

    ma_device device;
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cerr << "\nFehler: Konnte ALSA Audio-Gerät nicht initialisieren." << std::endl;
        return -1;
    }
    ma_device_start(&device);

    std::thread audioThread(audioProcessingLoop, &dspEngine, mode, filePath, &sharedBuffer, std::ref(isRunning));
    
    std::thread webThread(webServerLoop, &dspEngine, std::ref(isRunning));
    webThread.detach();

    std::cout << "\n=== Steuerung (Live) ===" << std::endl;
    std::cout << " Du kannst das System ab sofort auch ueber den Browser steuern!" << std::endl;
    std::cout << " [q] : Beenden" << std::endl;
    std::cout << "========================\n" << std::endl;

    while (isRunning) {
        int key = getKeyboardInput();
        if (key == 'q') isRunning = false;
        
        printStatus(dspEngine.getVolume(), dspEngine.getDn(), dspEngine.getAttenuation(), dspEngine.getCenterP(), dspEngine.getRaceEnabled(), dspEngine.getFiltersEnabled());
        usleep(500000); 
    }

    std::cout << "\n\nFahre Engine herunter..." << std::endl;
    
    if (audioThread.joinable()) audioThread.join();
    ma_device_uninit(&device);
    return 0;
}
