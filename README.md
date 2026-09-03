# OAPW (Ambiophonics Audio Engine) - Version 11

**Entwickelt von:** Dr. Ulrich Thibaut
**Plattform:** C++17 / Optimiert für ARM (Raspberry Pi 5) & x86/ARM64

## Über das Projekt
OAPW ist eine hocheffiziente, block-basierte Echtzeit-Audio-Engine. Sie implementiert den "Recursive Ambiophonic Crosstalk Elimination" (RACE) Algorithmus nach Ralph Glasgal, um das akustische Übersprechen bei einer regulären Stereo-Lautsprecheraufstellung zu reduzieren und eine dreidimensionale, holografische Wiedergabe zu erzielen.

Für eine kurze Einführung in die Ambiophonie als eine der reinen Stereophonie in Transparenz und verbesserter räumlicher Abbildung weit überlegene audiophile Wiedergabetechnik in hochwertigen Stereo-Anlagen siehe auch die beiliegende README.txt sowie die dort zitierte Original-Literatur von Ralph Glasgal et al.

## Aktueller Stand: Psychoakustik-Update (V11)
Die Architektur wurde vollständig überarbeitet, um klangliche Transparenz und numerische Stabilität zu maximieren:

1. **Psychoakustisches Head-Shadowing:** Statt aggressiver Tiefpässe simuliert ein sanfter High-Shelf-Filter (-12 dB ab 2000 Hz) die natürliche Schallabsorption des menschlichen Kopfes. Dies erhält die Transienten und verhindert tonale Verfärbungen ("Knistern") bei symphonischen Pegelspitzen.
2. **Phasenlineares Crossover:** Ein einzelner Butterworth-Hochpass (12 dB/Oktave bei 150 Hz) schützt den Bassbereich vor Auslöschungen und halbiert die Phasendrehungen im psychoakustisch kritischen Grundtonbereich.
3. **Fraktionale Verzögerung (Sub-Sample):** Die Ringpuffer-Architektur nutzt nun eine hochpräzise 4-Punkt Hermite-Interpolation. Dies ermöglicht exakte Laufzeitanpassungen, ohne die Hochtonenergie im Übersprechsignal zu beschneiden.

## Kern-Features
* **Echtzeit-Audioausgabe:** Native Ansteuerung des DAC HAT pro über ALSA mittels `miniaudio.h` (Push-Modus über Thread-sicheren Ringpuffer).
* **Dual-Mode Input:** 
   * Dateimodus (`.wav` und `.mp3` via `dr_wav.h` und `dr_mp3.h`).
   * Stream-Modus (Named Pipes via `/tmp/oapw_stream`), konfiguriert für Shairport Sync (AirPlay).
* **Live-Steuerung:** Thread-sichere (Mutex) Anpassung aller DSP-Parameter (Volume, Delay, Attenuation, Center, RACE-Bypass) in Echtzeit.
* **Web-GUI (Auto-Kalibrierung):** Integrierter, asynchroner Webserver (`httplib.h`) auf Port 8080 zur Headless-Steuerung aus dem Browser, inklusive Geometrie-Rechner für das akustische Setup der Stereoanlage.

---

## Systemvoraussetzungen & Hardware
* **Hardware:** Raspberry Pi 5 mit aufgesetztem **DAC HAT pro** (für native, hochauflösende Audioausgabe).
* **Betriebssystem:** Linux (z. B. Raspberry Pi OS).
* **Compiler:** C++17-kompatibler Compiler (GCC 9+ oder Clang) sowie `cmake`.
* **Tools:** `ffmpeg` (für externe Audio-Zuspielung per Skript).
* **Bibliotheken:** ALSA-Entwicklungspakete (`libasound2-dev`).

## Installation und Kompilierung
Das Projekt nutzt CMake für den Build-Prozess und kann nativ auf dem Raspberry Pi 5 gebaut werden.

```bash
# 1. Repository klonen
git clone https://github.com/DEIN_USERNAME/oapw.git
cd oapw

# 2. Abhängigkeiten installieren
sudo apt-get update
sudo apt-get install build-essential cmake libasound2-dev ffmpeg

# 3. Build-Verzeichnis erstellen und kompilieren
mkdir build
cd build
cmake ..
make
```
Das kompilierte Binary `OAPW_Player` befindet sich anschließend im `build/`-Verzeichnis.

## Nutzung & Audio-Streaming

**1. Engine starten**
Um die Audio-Engine im Stream-Modus zu starten und die Named Pipe anzulegen:
```bash
./build/OAPW_Player --stream
```
Der Webserver ist nun unter `http://<IP-des-Raspberry>:8080` erreichbar.

**2. Zuspielung via `oapw-play`**
Für die nahtlose Zuspielung lokaler Audio-Bibliotheken in die Pipe (`/tmp/oapw_stream`) liegt dem Repository das Skript `oapw-play` bei. Es durchsucht Verzeichnisse rekursiv nach Audiodateien und decodiert sie passend für die Engine.

```bash
# Skript ausführbar machen und systemweit verlinken
chmod +x oapw_play
sudo cp oapw_play /usr/local/bin/oapw-play

# Einen ganzen Ordner in zufälliger Reihenfolge als Endlosschleife abspielen
oapw-play --shuffle --repeat ~/Music/HighRes
```

---

## Roadmap & Ports

### 1. Multi-Room Deployment (RPi4 und anderes Stereo-Equipment)
Das OAPW-System ist vollständig hardwareunabhängig. Das exakt gleiche Setup (Hintergrunddienst + Shairport Sync + Named Pipe) ist für den Einsatz auf einem Raspberry Pi 4 mit USB Sharkoon DAC vorgesehen, der an einen Yamaha-Vollverstärker mit passiven Canton-Lautsprechern angeschlossen ist. Da die Audio-Engine externe USB-DACs nahtlos unterstützt, kann der Code dort identisch kompiliert und als `oapw.service` im Hintergrund betrieben werden.

### 2. Nativer Port für macOS (Abgeschlossen)
Die Portierung der `RACEDspEngine` als native, eigenständige Desktop-App für **macOS Tahoe 26.2** ist bereits abgeschlossen. Diese Version ersetzt den integrierten Webserver durch eine native Benutzeroberfläche und klinkt sich für systemweite Audio-Streams direkt in Apples CoreAudio-APIs ein. 
*Der macOS-Port wird in Kürze in einem separaten GitHub-Repository bereitgestellt.*

### 3. Erweiterte Zuspielung
* Aufbau einer webbasierten Playlist-Funktion mit myMPD und FIFO-Pipe direkt in `/tmp/oapw_stream`. Diese Funktionalität läuft derzeit als Prototyp und wird nach erfolgreichem Abschluss der Tests ebenfalls auf GitHub zur Verfügung gestellt.
