# Projekt OAPW (Ambiophonics Audio Engine)

## Aktueller Stand (Meilenstein 5 = Version 12 abgeschlossen)
Das Projekt ist eine in modernem C++ (C++17) geschriebene, block-basierte Echtzeit-Audio-Engine für den Raspberry Pi 5. Sie implementiert den "Recursive Ambiophonic Crosstalk Elimination" (RACE) Algorithmus von Ralph Glasgal unter Verwendung von hochpräzisen 19-Pol IIR-Filtern (64-Bit double) zur Frequenzlimitierung. Zusätzlich wurde in der aktuellen Version 12 noch ein parametrischer Equalizer für drei getrennte Frequenzbänder eingefügt (Hoch - Mittel - Tief), der ebenfalls über das Web-Frontend bedient werden kann, um den Frequenzgang an den Lautsprechern für die jeweils gegebene Räumlichkeit zu optimieren. 

## Implementierte Features
1. **Echtzeit-Audioausgabe:** Native Ansteuerung des DAC HAT pro über ALSA mittels `miniaudio.h` (Push-Modus über Thread-sicheren Ringpuffer).
2. **Dual-Mode Input:** 
   - Dateimodus (`.wav` und `.mp3` via `dr_wav.h` und `dr_mp3.h`).
   - Stream-Modus (Named Pipes via `/tmp/oapw_stream`), vorbereitet für AirPlay/Shairport Sync.
3. **Live-Steuerung:** Thread-sichere (Mutex) Anpassung von RACE-Parametern (Volume, Delay, Attenuation, Center, RACE-Bypass) sowie Frequenzgang im Hoch- Mittel- und Tieftonbereich in Echtzeit während der Wiedergabe.
4. **Web-GUI:** Integrierter, asynchroner Webserver (`httplib.h`) auf Port 8080 zur Headless-Steuerung aus dem Browser.
5. **AirPlay** Konfiguration von Shairport Sync für die direkte Einspeisung des AirPlay-Streams in die Named Pipe `/tmp/oapw_stream`.
 
## Nächster geplanter Entwicklungsschritt
- (Optional) Aufbau einer integrierten Playlist-Funktion.

=========================================================
ROADMAP & ZUKÜNFTIGE ERWEITERUNGEN
=========================================================

1. Nativer Port für macOS (Eigenständige App, bereits als Beta-Version auf GitHub veröffentlicht)
Da die RACEDspEngine in reinem, standardisiertem C++17 geschrieben und strikt von der Audio-Ausgabe und dem Webserver getrennt ist, lässt sich der Rechenkern 1:1 auf Apple-Hardware übertragen. Ein zukünftiges Ziel ist eine gekapselte, eigenständige Desktop-App unter macOS Tahoe 26.2. Diese würde den Webserver durch eine native Benutzeroberfläche (z. B. via SwiftUI oder Objective-C++) ersetzen und systemweite Audio-Streams direkt auf dem Mac verarbeiten.
=========================================================
