# OAPW: Open Ambiophonics Processing Wrapper

OAPW ist eine hochperformante Audio-Engine zur Echtzeit-Berechnung des Recursive Ambiophonic Crosstalk Elimination (RACE) Algorithmus, ergänzt um dynamische DSP-Raumkorrektur.

## 1. Ambiophonie & Crosstalk Cancellation
Die klassische Stereophonie leidet physikalisch bedingt unter interauralem Übersprechen (Crosstalk): Das linke Ohr registriert nicht nur das dedizierte linke Signal, sondern zeitverzögert und abgeschwächt auch den Schall des rechten Lautsprechers. Dies verschleiert die psychoakustische Ortung. OAPW eliminiert diesen Fehler, indem das Signal präzise verzögert, invertiert und beigemischt wird. Das Resultat ist eine holografische, dreidimensionale Klangbühne.

Die ideale Laufzeitdifferenz (Δt) berechnet sich aus dem Abstand zum ipsilateralen (d_ipsi) und kontralateralen (d_contra) Ohr unter Einbezug der Schallgeschwindigkeit (c):
Δt = (d_contra - d_ipsi) / c

**Vektor-Modell des Signalflusses:**
   [ Lautsprecher L ]        [ Lautsprecher R ]
           |  \                    /  |
  Direkt   |    \ Crosstalk      /    | Direkt
           |      \            /      |
           |        \        /        |
           v          X            v
        [Linkes Ohr]          [Rechtes Ohr]

## 2. Parametrische Raumkorrektur (PEQ)
Um die Ambiophonie-Bühne von physischen Raumresonanzen (Raummoden) zu befreien, durchläuft das Signal anschließend kaskadierte Biquad-IIR-Filter. Jeder EQ-Knotenpunkt steuert die Amplitude und die Filtergüte (Q), welche die Bandbreite exakt um die Mittenfrequenz (f0) definiert.

## 3. Architektur & Betrieb (oapw.service)
Das System läuft auf headless Linux-Systemen (wie Raspberry Pi 4 und 5) als ressourcenschonender Hintergrunddienst. Das Web-GUI ist lokal über Port 8080 erreichbar. OAPW greift exklusiv auf den ALSA-Hardware-Wandler zu.

* **Player stoppen:** `sudo systemctl stop oapw`
* **Player starten:** `sudo systemctl start oapw`
* **Player neustarten:** `sudo systemctl restart oapw`
* **Status prüfen:** `sudo systemctl status oapw`
* **Live-Logbuch:** `journalctl -u oapw -f`

**AirPlay-Integration:** 
Der `shairport-sync` Dienst leitet den Stream unter Umgehung von ALSA direkt in die Pipe `/tmp/oapw_stream`.

## 4. Roadmap
* **Hardware-Unabhängigkeit:** Das Setup operiert identisch über aufgesteckte DAC-HATs (Raspberry Pi 5) oder externe USB-Wandler (Raspberry Pi 4).
* **Native macOS Portierung:** Die Trennung von DSP-Kern und UI ermöglicht zukünftige Standalone-Apps. Eine native Portierung für systemweites Routing unter macOS Tahoe 26.2 befindet sich im Beta-Status.

---

# English Summary: DSP Theory & Architecture

**Ambiophonics & RACE**
Traditional stereophonic reproduction is fundamentally limited by interaural crosstalk—the phenomenon where the left ear hears not only the left speaker but also the delayed and attenuated sound from the right speaker. This physical limitation compromises psychoacoustic localization and narrows the soundstage. OAPW addresses this via the Recursive Ambiophonic Crosstalk Elimination (RACE) algorithm. By precisely calculating the Interaural Time Difference (ITD) based on listener geometry and applying an inverted, delayed cancellation signal, OAPW creates a three-dimensional, holographic soundstage. 

**Parametric EQ (PEQ)**
To compensate for room modes (acoustic resonances), the DSP chain incorporates a cascading Biquad IIR filter stage. This allows precise tuning of specific frequencies by adjusting the gain and Q-factor (bandwidth), ensuring the ambiophonic field remains transparent and uncolored by the physical listening room.

**System Architecture**
OAPW is designed in C++17 as a headless, cross-platform audio processing engine. On Linux distributions (e.g., Raspberry Pi), it runs as a background daemon capturing streams via a POSIX named pipe (ideal for Shairport Sync / AirPlay integration) and outputs directly to ALSA-compliant DACs. Its decoupled DSP core enables straightforward porting to other platforms, including an upcoming native release for macOS Tahoe 26.2.
