# OAPW (Ambiophonics Audio Engine) - Version 11

**Entwickelt von:** Dr. Ulrich Thibaut
**Plattform:** C++17 / Optimiert für ARM (Raspberry Pi 5) & x86/ARM64

## Über das Projekt
OAPW ist eine hocheffiziente, block-basierte Echtzeit-Audio-Engine. Sie implementiert den "Recursive Ambiophonic Crosstalk Elimination" (RACE) Algorithmus nach Ralph Glasgal, um das akustische Übersprechen bei einer regulären Stereo-Lautsprecheraufstellung zu reduzieren und eine dreidimensionale, holografische Wiedergabe zu erzielen.

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
* **Web-GUI (Auto-Kalibrierung):** Integrierter, asynchroner Webserver (`httplib.h`) auf Port 8080 zur Headless-Steuerung aus dem Browser, inklusive Geometrie-Rechner für das akustische Setup.

---

## Roadmap & Zukünftige Erweiterungen

### 1. Multi-Room Deployment (RPi4 und anderes Stereo-Equipment)
Das OAPW-System ist vollständig hardwareunabhängig. Das exakt gleiche Setup (Hintergrunddienst + Shairport Sync + Named Pipe) ist für den Einsatz auf einem Raspberry Pi 4 mit USB Sharkoon DAC vorgesehen, der an einen Yamaha-Vollverstärker mit passiven Canton-Lautsprechern angeschlossen ist. Da die Audio-Engine externe USB-DACs nahtlos unterstützt, kann der Code dort identisch kompiliert und als `oapw.service` im Hintergrund betrieben werden.

### 2. Nativer Port für macOS (Eigenständige App)
Da die `RACEDspEngine` in reinem, standardisiertem C++17 geschrieben und strikt von der Audio-Ausgabe und dem Webserver getrennt ist, lässt sich der Rechenkern 1:1 auf Apple-Hardware übertragen. Ein zukünftiges Ziel ist eine gekapselte, eigenständige Desktop-App unter macOS Tahoe 26.2. Diese wird den Webserver durch eine native Benutzeroberfläche ersetzen und systemweite Audio-Streams über Apples CoreAudio-APIs direkt verarbeiten.

### 3. Erweiterte Zuspielung, z.B. PlayList Funktion
* Aufbau einer webbasierten Playlist-Funktion mit myMPD und FIFO-Pipe direkt in `/tmp/oapw_stream`.
