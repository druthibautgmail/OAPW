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
        .calc-group { border: 1px solid #007aff; }
        .eq-band { border-top: 1px solid #333; padding-top: 1rem; margin-top: 1rem; }
        label { display: flex; justify-content: space-between; margin-bottom: 0.5rem; font-weight: bold; font-size: 0.9rem; }
        input[type="range"] { width: 100%; cursor: pointer; margin-bottom: 1rem; }
        input[type="number"] { width: 100%; padding: 10px; margin-bottom: 15px; background: #333; color: white; border: none; border-radius: 4px; box-sizing: border-box; font-size: 1rem; }
        .value-display { font-weight: normal; color: #aaa; }
        button { width: 100%; padding: 15px; font-size: 1.2rem; font-weight: bold; border: none; border-radius: 8px; cursor: pointer; background: #007aff; color: white; margin-bottom: 1.0rem; transition: background 0.3s;}
        button.off { background: #333; color: #888; }
        h3 { margin-top: 0; margin-bottom: 1rem; font-weight: 500; color: #007aff; }
        h4 { margin-top: 0; margin-bottom: 1rem; color: #fff; font-size: 1rem; }
    </style>
</head>
<body>
    <h1>OAPW Ambiophonics DSP</h1>
    
    <button id="btnRace" onclick="toggleRace()">RACE: AN</button>
    <button id="btnFilters" onclick="toggleFilters()">Filter: AN</button>
    <button id="btnEq" onclick="toggleEq()">EQ: AUS</button>

    <div class="control-group calc-group">
        <h3>Geometrie-Rechner</h3>
        <label>Abstand Lautsprecher (Mitte-Mitte in cm)</label>
        <input type="number" id="spkDist" value="30">
        <label>Abstand Hörer zur Basislinie (in cm)</label>
        <input type="number" id="listDist" value="80">
        <button onclick="calcGeometry()" style="margin-bottom: 0; background: #0a84ff;">Berechnen & Anwenden</button>
    </div>

    <!-- Parameter Sektion -->
    <div class="control-group">
        <h3>DSP Parameter</h3>
        <label>Lautstärke <span id="valVol" class="value-display">50%</span></label>
        <input type="range" id="vol" min="0" max="100" value="50" oninput="updateParam('vol', this.value)">

        <label>Interaural Delay <span id="valDelay" class="value-display">68 &micro;s</span></label>
        <input type="range" id="delay" min="20" max="1500" step="2" value="68" oninput="updateParam('delay', this.value)">

        <label>Attenuation <span id="valAtt" class="value-display">-2.3 dB</span></label>
        <input type="range" id="att" min="-30" max="0" step="0.1" value="-2.3" oninput="updateParam('att', this.value)">

        <label>Center Parameter <span id="valCenter" class="value-display">0.10</span></label>
        <input type="range" id="center" min="0" max="100" value="10" oninput="updateParam('center', this.value)">
    </div>

    <!-- EQ Sektion -->
    <div class="control-group">
        <h3>Parametrischer Equalizer (PEQ)</h3>
        
        <div class="eq-band">
            <h4>Band 1 (Bass)</h4>
            <label>Frequenz <span id="valEq0F" class="value-display">100 Hz</span></label>
            <input type="range" id="eq0F" min="20" max="500" step="1" value="100" oninput="updateEq(0, 'f', this.value)">
            <label>Q-Faktor <span id="valEq0Q" class="value-display">0.7</span></label>
            <input type="range" id="eq0Q" min="0.1" max="5.0" step="0.1" value="0.7" oninput="updateEq(0, 'q', this.value)">
            <label>Gain <span id="valEq0G" class="value-display">0.0 dB</span></label>
            <input type="range" id="eq0G" min="-15" max="15" step="0.5" value="0" oninput="updateEq(0, 'g', this.value)">
        </div>

        <div class="eq-band">
            <h4>Band 2 (Mitten)</h4>
            <label>Frequenz <span id="valEq1F" class="value-display">1000 Hz</span></label>
            <input type="range" id="eq1F" min="200" max="5000" step="10" value="1000" oninput="updateEq(1, 'f', this.value)">
            <label>Q-Faktor <span id="valEq1Q" class="value-display">0.7</span></label>
            <input type="range" id="eq1Q" min="0.1" max="5.0" step="0.1" value="0.7" oninput="updateEq(1, 'q', this.value)">
            <label>Gain <span id="valEq1G" class="value-display">0.0 dB</span></label>
            <input type="range" id="eq1G" min="-15" max="15" step="0.5" value="0" oninput="updateEq(1, 'g', this.value)">
        </div>

        <div class="eq-band">
            <h4>Band 3 (Höhen)</h4>
            <label>Frequenz <span id="valEq2F" class="value-display">5000 Hz</span></label>
            <input type="range" id="eq2F" min="2000" max="20000" step="100" value="5000" oninput="updateEq(2, 'f', this.value)">
            <label>Q-Faktor <span id="valEq2Q" class="value-display">0.7</span></label>
            <input type="range" id="eq2Q" min="0.1" max="5.0" step="0.1" value="0.7" oninput="updateEq(2, 'q', this.value)">
            <label>Gain <span id="valEq2G" class="value-display">0.0 dB</span></label>
            <input type="range" id="eq2G" min="-15" max="15" step="0.5" value="0" oninput="updateEq(2, 'g', this.value)">
        </div>
    </div>

    <script>
        let raceActive = true;
        let filtersActive = true;
        let eqActive = false;

        function updateParam(param, value) {
            if(param === 'vol') document.getElementById('valVol').innerHTML = value + '%';
            if(param === 'delay') document.getElementById('valDelay').innerHTML = value + ' &micro;s';
            if(param === 'att') document.getElementById('valAtt').innerHTML = parseFloat(value).toFixed(1) + ' dB';
            if(param === 'center') document.getElementById('valCenter').innerHTML = (value/100).toFixed(2);
            fetch(`/api/update?${param}=${value}`);
        }

        function updateEq(band, param, value) {
            let labelId = 'valEq' + band + param.toUpperCase();
            if(param === 'f') document.getElementById(labelId).innerHTML = value + ' Hz';
            if(param === 'q') document.getElementById(labelId).innerHTML = parseFloat(value).toFixed(1);
            if(param === 'g') document.getElementById(labelId).innerHTML = parseFloat(value).toFixed(1) + ' dB';
            fetch(`/api/update?eq${band}${param}=${value}`);
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

        function toggleEq() {
            eqActive = !eqActive;
            let btn = document.getElementById('btnEq');
            btn.innerText = eqActive ? "EQ: AN" : "EQ: AUS";
            btn.className = eqActive ? "" : "off";
            fetch(`/api/update?eq=${eqActive ? 1 : 0}`);
        }

        function calcGeometry() {
            let ds = parseFloat(document.getElementById('spkDist').value);
            let dl = parseFloat(document.getElementById('listDist').value);
            if(isNaN(ds) || isNaN(dl) || ds <= 0 || dl <= 0) return;
            let earDist = 17.5; 
            let c = 34.32;      
            let d_ipsi = Math.sqrt(Math.pow(dl, 2) + Math.pow(ds/2 - earDist/2, 2));
            let d_contra = Math.sqrt(Math.pow(dl, 2) + Math.pow(ds/2 + earDist/2, 2));
            let delta_d = d_contra - d_ipsi;
            let idealDelay = Math.round((delta_d / c) * 1000); 
            let geoLoss = 20 * Math.log10(d_ipsi / d_contra);
            let idealAtt = geoLoss - (idealDelay * 0.015); 
            if (idealDelay > 1500) idealDelay = 1500;
            if (idealDelay < 20) idealDelay = 20;
            if (idealAtt < -30) idealAtt = -30;
            if (idealAtt > 0) idealAtt = 0;
            document.getElementById('delay').value = idealDelay;
            updateParam('delay', idealDelay);
            let attRounded = (Math.round(idealAtt * 10) / 10).toFixed(1);
            document.getElementById('att').value = attRounded;
            updateParam('att', attRounded);
        }

        window.onload = () => {
            fetch('/api/status').then(r => r.json()).then(data => {
                document.getElementById('vol').value = data.volume * 100;
                document.getElementById('valVol').innerHTML = Math.round(data.volume * 100) + '%';
                
                document.getElementById('delay').value = data.delayUs;
                document.getElementById('valDelay').innerHTML = data.delayUs + ' &micro;s';
                
                document.getElementById('att').value = data.attenuationDb;
                document.getElementById('valAtt').innerHTML = data.attenuationDb.toFixed(1) + ' dB';
                
                document.getElementById('center').value = data.centerP * 100;
                document.getElementById('valCenter').innerHTML = data.centerP.toFixed(2);

                raceActive = data.raceActive;
                document.getElementById('btnRace').innerText = raceActive ? "RACE: AN" : "RACE: AUS";
                document.getElementById('btnRace').className = raceActive ? "" : "off";

                filtersActive = data.filtersActive;
                document.getElementById('btnFilters').innerText = filtersActive ? "Filter: AN" : "Filter: AUS";
                document.getElementById('btnFilters').className = filtersActive ? "" : "off";

                eqActive = data.eqActive;
                document.getElementById('btnEq').innerText = eqActive ? "EQ: AN" : "EQ: AUS";
                document.getElementById('btnEq').className = eqActive ? "" : "off";

                for(let b=0; b<3; b++) {
                    document.getElementById('eq'+b+'F').value = data.eq[b].f;
                    document.getElementById('valEq'+b+'F').innerHTML = data.eq[b].f + ' Hz';
                    document.getElementById('eq'+b+'Q').value = data.eq[b].q;
                    document.getElementById('valEq'+b+'Q').innerHTML = data.eq[b].q.toFixed(1);
                    document.getElementById('eq'+b+'G').value = data.eq[b].g;
                    document.getElementById('valEq'+b+'G').innerHTML = data.eq[b].g.toFixed(1) + ' dB';
                }
            });
        };
    </script>
</body>
</html>
)HTML";

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

void printStatus(float vol, float delayUs, float attDb, float center, bool raceActive, bool filtersActive, bool eqActive) {
    std::cout << "\r[Status] Vol: " << (int)(vol * 100) << "% | "
              << "RACE: " << (raceActive ? "AN" : "AUS") << " | "
              << "Filter: " << (filtersActive ? "AN" : "AUS") << " | "
              << "EQ: " << (eqActive ? "AN" : "AUS") << " | "
              << "Delay: " << delayUs << " us | "
              << "Att: " << attDb << " dB      " << std::flush;
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

void audioProcessingLoop(RACEDspEngine* dspEngine, std::string mode, std::string filePath, ThreadSafeAudioBuffer* sharedBuffer, std::atomic<bool>& isRunning) {
    const size_t maxBufferSize = 44100 * 2 * 2; 

    if (mode == "stream") {
        const char* pipeName = "/tmp/oapw_stream";
        mkfifo(pipeName, 0666); 
        chmod(pipeName, 0666);
        
        int fd = -1; 
        std::vector<int16_t> intBuf(1024 * 2);
        std::vector<float> floatBuf(1024 * 2);

        while (isRunning) {
            if (fd < 0) {
                fd = open(pipeName, O_RDONLY);
                if (fd < 0) { usleep(100000); continue; }
            }

            if (sharedBuffer->size() > maxBufferSize) { usleep(10000); continue; }
            ssize_t bytesRead = read(fd, intBuf.data(), intBuf.size() * sizeof(int16_t));
            
            if (bytesRead > 0) {
                size_t samplesRead = bytesRead / sizeof(int16_t);
                size_t validSamples = (samplesRead / 2) * 2; 
                if (validSamples > 0) {
                    for (size_t i = 0; i < validSamples; ++i) floatBuf[i] = intBuf[i] / 32768.0f; 
                    std::vector<float> chunk(floatBuf.begin(), floatBuf.begin() + validSamples);
                    dspEngine->processSamples(chunk);
                    sharedBuffer->push(chunk);
                }
            } else if (bytesRead == 0) {
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

void webServerLoop(RACEDspEngine* dspEngine, std::atomic<bool>& isRunning) {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(HTML_CONTENT, "text/html");
    });

    svr.Get("/api/status", [dspEngine](const httplib::Request&, httplib::Response& res) {
        std::string json = "{";
        json += "\"volume\":" + std::to_string(dspEngine->getVolume()) + ",";
        json += "\"delayUs\":" + std::to_string(dspEngine->getDelayUs()) + ",";
        json += "\"attenuationDb\":" + std::to_string(dspEngine->getAttenuationDb()) + ",";
        json += "\"centerP\":" + std::to_string(dspEngine->getCenterP()) + ",";
        json += "\"raceActive\":" + std::string(dspEngine->getRaceEnabled() ? "true" : "false") + ",";
        json += "\"filtersActive\":" + std::string(dspEngine->getFiltersEnabled() ? "true" : "false") + ",";
        json += "\"eqActive\":" + std::string(dspEngine->getEqEnabled() ? "true" : "false") + ",";
        json += "\"eq\":[";
        for(int b=0; b<3; b++) {
            json += "{\"f\":" + std::to_string(dspEngine->getEqFreq(b)) + 
                    ",\"q\":" + std::to_string(dspEngine->getEqQ(b)) + 
                    ",\"g\":" + std::to_string(dspEngine->getEqGain(b)) + "}";
            if(b < 2) json += ",";
        }
        json += "]}";
        res.set_content(json, "application/json");
    });

    svr.Get("/api/update", [dspEngine](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("vol")) dspEngine->setVolume(std::stof(req.get_param_value("vol")) / 100.0f);
        if (req.has_param("race")) dspEngine->setRaceEnabled(std::stoi(req.get_param_value("race")) != 0);
        if (req.has_param("filters")) dspEngine->setFiltersEnabled(std::stoi(req.get_param_value("filters")) != 0);
        if (req.has_param("eq")) dspEngine->setEqEnabled(std::stoi(req.get_param_value("eq")) != 0);

        if (req.has_param("delay") || req.has_param("att") || req.has_param("center")) {
            float delayUs = dspEngine->getDelayUs();
            float attDb = dspEngine->getAttenuationDb();
            float center = dspEngine->getCenterP();
            if (req.has_param("delay")) delayUs = std::stof(req.get_param_value("delay"));
            if (req.has_param("att")) attDb = std::stof(req.get_param_value("att"));
            if (req.has_param("center")) center = std::stof(req.get_param_value("center")) / 100.0f;
            dspEngine->setParameters(delayUs, attDb, center, dspEngine->getFreqLimit());
        }

        // Dynamisches Verarbeiten der EQ Baender (eq0f, eq0q, eq0g, etc.)
        for(int b=0; b<3; b++) {
            std::string pf = "eq" + std::to_string(b) + "f";
            std::string pq = "eq" + std::to_string(b) + "q";
            std::string pg = "eq" + std::to_string(b) + "g";
            if (req.has_param(pf) || req.has_param(pq) || req.has_param(pg)) {
                float f = req.has_param(pf) ? std::stof(req.get_param_value(pf)) : dspEngine->getEqFreq(b);
                float q = req.has_param(pq) ? std::stof(req.get_param_value(pq)) : dspEngine->getEqQ(b);
                float g = req.has_param(pg) ? std::stof(req.get_param_value(pg)) : dspEngine->getEqGain(b);
                dspEngine->setEqBand(b, f, q, g);
            }
        }
        res.set_content("OK", "text/plain");
    });

    std::cout << "\n[Web] Web-Interface gestartet auf Port 8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
}

int main(int argc, char** argv) {
    std::string mode = "stream";
    std::string filePath = "";
    if (argc == 2) { mode = "file"; filePath = argv[1]; }

    std::atomic<bool> isRunning(true);
    
    RACEDspEngine dspEngine(68.0f, -2.3f, 0.1f, true);
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
        std::cerr << "\nFehler: Konnte ALSA Audio-Geraet nicht initialisieren." << std::endl;
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
        
        printStatus(dspEngine.getVolume(), dspEngine.getDelayUs(), dspEngine.getAttenuationDb(), dspEngine.getCenterP(), dspEngine.getRaceEnabled(), dspEngine.getFiltersEnabled(), dspEngine.getEqEnabled());
        usleep(500000); 
    }

    std::cout << "\n\nFahre Engine herunter..." << std::endl;
    
    if (audioThread.joinable()) audioThread.join();
    ma_device_uninit(&device);
    return 0;
}
