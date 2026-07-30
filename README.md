# Projekt OAPW (Ambiophonics Audio Engine)

## Aktueller Stand (Meilenstein 4 abgeschlossen)
Das Projekt ist eine in modernem C++ (C++17) geschriebene, block-basierte Echtzeit-Audio-Engine für den Raspberry Pi 5. Sie implementiert den "Recursive Ambiophonic Crosstalk Elimination" (RACE) Algorithmus von Ralph Glasgal unter Verwendung von hochpräzisen 19-Pol IIR-Filtern (64-Bit double) zur Frequenzlimitierung.

## Implementierte Features
1. **Echtzeit-Audioausgabe:** Native Ansteuerung des DAC HAT pro über ALSA mittels `miniaudio.h` (Push-Modus über Thread-sicheren Ringpuffer).
2. **Dual-Mode Input:** 
   - Dateimodus (`.wav` und `.mp3` via `dr_wav.h` und `dr_mp3.h`).
   - Stream-Modus (Named Pipes via `/tmp/oapw_stream`), vorbereitet für AirPlay/Shairport Sync.
3. **Live-Steuerung:** Thread-sichere (Mutex) Anpassung von Parametern (Volume, Delay, Attenuation, Center, RACE-Bypass) während der Wiedergabe.
4. **Web-GUI:** Integrierter, asynchroner Webserver (`httplib.h`) auf Port 8080 zur Headless-Steuerung aus dem Browser.
5. **AirPlay** Konfiguration von Shairport Sync für die direkte Einspeisung des AirPlay-Streams in die Named Pipe `/tmp/oapw_stream`.
# Ambiophonics RACE DSP Engine (macOS Build)

## Architektur & Signalfluss neu aufgebaut am 29.07.2026
Dieses Modul implementiert den Recursive Ambiophonic Crosstalk Elimination (RACE) Algorithmus nach Ralph Glasgal für eine Abtastrate von 44.1 kHz.

1. **Ringpuffer-Delay:** Erzeugt ein präzises Sample-Delay (Standard: 3 Samples entsprechen ~68 µsec bei 44.1 kHz).
2. **Matrix-Kombination:** Berechnet die kanalübergreifende Signalauslöschung.
3. **Phasenlineares Dry/Wet-Bandpass-Prinzip:** 
   - Das RACE-Signal wird als Differenz zum Eingangssignal isoliert.
   - Ein kaskadiertes Butterworth-Filter 4. Ordnung (24 dB/Oktabteilung bei 150 Hz Highpass und 5000 Hz Lowpass) beschränkt die Auslöschung exakt auf den psychoakustisch relevanten Mittenbereich.
   - Signalanteile außerhalb dieses Fensters (tiefe Bässe und feine Höhen) passieren den DSP-Kern zu 100% phasenrein und unberührt. 
## Nächster geplanter Entwicklungsschritt
- (Optional) Aufbau einer Web-basierten Playlist-Funktion mit myMPD und fifo pipe direkt in /tmp/oapw_stream.

=========================================================
ROADMAP & ZUKÜNFTIGE ERWEITERUNGEN
=========================================================

1. Multi-Room Deployment (Spa-Bereich)
Das OAPW-System ist vollständig hardwareunabhängig. Das exakt gleiche Setup (Hintergrunddienst + Shairport Sync + Named Pipe) ist für den Einsatz auf einem Raspberry Pi 4 vorgesehen, der an einen Yamaha Vollverstärker mit passiven Canton Lautsprechern im Spa angeschlossen ist. Da die Audio-Engine externe USB-DACs nahtlos unterstützt, kann der Code dort identisch kompiliert und als "oapw.service" im Hintergrund betrieben werden.

2. Nativer Port für macOS (Eigenständige App)
Da die RACEDspEngine in reinem, standardisiertem C++17 geschrieben und strikt von der Audio-Ausgabe und dem Webserver getrennt ist, lässt sich der Rechenkern 1:1 auf Apple-Hardware übertragen. Ein zukünftiges Ziel ist eine gekapselte, eigenständige Desktop-App unter macOS Tahoe 26.2. Diese würde den Webserver durch eine native Benutzeroberfläche (z. B. via SwiftUI oder Objective-C++) ersetzen und systemweite Audio-Streams direkt auf dem Mac verarbeiten.
=========================================================
