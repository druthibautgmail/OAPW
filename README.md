# OAPW (Ambiophonics Audio Engine) - Version 15.1

**Entwickelt von:** Dr. Ulrich Thibaut
**Plattform:** C++17 / Cross-Plattform (Raspberry Pi 5, Raspberry Pi 4, macOS)

## Über das Projekt
OAPW ist eine hocheffiziente, block-basierte Echtzeit-Audio-Engine. Sie implementiert den "Recursive Ambiophonic Crosstalk Elimination" (RACE) Algorithmus nach Ralph Glasgal, um das akustische Übersprechen bei einer regulären Stereo-Lautsprecheraufstellung zu reduzieren und eine dreidimensionale, holografische Wiedergabe zu erzielen.

Für eine kurze Einführung in die Ambiophonie als audiophile Wiedergabetechnik siehe die beiliegende README.txt sowie die dort zitierte Original-Literatur von Ralph Glasgal et al.

## Aktueller Stand: Cross-Plattform, Recording & EQ (V15.1)
Die Architektur wurde über die vergangenen Versionen massiv ausgebaut, um als robustes, geräteübergreifendes Multi-Room-System zu fungieren:

1. **Dynamische Hardware-Erkennung (CMake):** Der Build-Prozess erkennt nun automatisch die zugrunde liegende CPU-Architektur (`-mcpu=native`). Das Projekt kompiliert ohne Code-Änderungen nativ auf Raspberry Pi 5 (Cortex-A76), Raspberry Pi 4 (Cortex-A72) und macOS.
2. **Parametrischer Equalizer (DSP):** Die RACE-Engine verfügt nun über eine zuschaltbare EQ-Stufe, um raumakustische Moden (z.B. wandnahe Eck-Aufstellung) präzise auszugleichen.
3. **Robuster Argument-Parser:** Ein neu geschriebener Parser erlaubt die flexible und fehlertolerante Kombination von Eingabe-Streams und parallelen WAV-Aufnahmen.
4. **Live Audio-Recording:** Der DSP-Output kann parallel zur Audioausgabe bitgenau als `.wav`-Datei auf der Festplatte mitgeschnitten werden.

## Kern-Features
* **Echtzeit-Audioausgabe:** Native Hardware-Ansteuerung via `miniaudio.h`. Unterstützt ALSA (I2S DAC HATs, USB-DACs) unter Linux sowie CoreAudio unter macOS.
* **Dual-Mode Input:** 
   * Dateimodus (`.wav` und `.mp3` via `dr_wav.h` und `dr_mp3.h`).
   * Stream-Modus (Named Pipes), optimiert für Shairport Sync (AirPlay).
* **Live-Steuerung:** Thread-sichere Anpassung aller DSP-Parameter (Volume, Delay, Attenuation, EQ, Center) in Echtzeit.
* **Web-GUI:** Integrierter asynchroner Webserver (`httplib.h`) auf Port 8080 zur grafischen Headless-Steuerung aus dem Browser.

---

## Systemvoraussetzungen & Hardware
* **Unterstützte Hardware:** 
  * Raspberry Pi 5 (z.B. mit Inno-Maker DAC HAT pro)
  * Raspberry Pi 4 (z.B. mit Sharkoon USB DAC)
  * Apple Mac (macOS Tahoe -- CAVE! Beta-Stadium, wird derzeit überarbeitet und als separates GitHub Release veröffentlicht!)
* **Compiler:** C++17-kompatibler Compiler (GCC 9+ oder Apple Clang) sowie `cmake`.
* **Bibliotheken (Linux):** ALSA-Entwicklungspakete (`libasound2-dev`).
* Shairport Sync als eine im Hintergrund auf dem RPi laufende Instanz ist empfohlen, um die Audiostreams über AirPlay zu empfangen.

## Installation und Kompilierung
Das Projekt nutzt CMake und kompiliert sich automatisch passend für das erkannte Host-System.

```bash
# 1. Repository klonen
git clone https://github.com/druthibautgmail/oapw.git
cd oapw

# 2. Abhängigkeiten installieren (nur Debian/Raspberry Pi OS)
sudo apt-get update
sudo apt-get install build-essential cmake libasound2-dev ffmpeg

# 3. Build-Verzeichnis erstellen und kompilieren
mkdir build
cd build
cmake ..
make -j4

---

## Nutzung als reiner Audio-Stream Prozessor

./build/OAPW_Player --stream /tmp/oapw_stream

## ... oder als Audio-Stream Prozessor und gleichzeitig zum Speichern des Audio-Streams als lokale .wav Datei (vor der Prozessierung!)

./build/OAPW_Player --stream /tmp/oapw_stream --record ~/test.wav
