=========================================================
OAPW - Hintergrunddienst & Audio-Routing (Stand: Mitte 2026)
=========================================================

1. DER HINTERGRUNDDIENST (oapw.service)
Unser C++ OAPW-Player läuft permanent als unsichtbarer Hintergrunddienst auf dem Raspberry Pi 5. 
Er horcht auf die Named Pipe (/tmp/oapw_stream) und gibt das Signal echtzeitnah über ALSA an den DAC aus. 
Das Web-Interface ist immer über Port 8080 erreichbar.

WICHTIGE BEFEHLE FÜR DAS TERMINAL:
- Player manuell stoppen:   sudo systemctl stop oapw
- Player manuell starten:   sudo systemctl start oapw
- Player neu starten:       sudo systemctl restart oapw
- Status/Fehler anzeigen:   sudo systemctl status oapw
- Live-Logbuch ansehen:     journalctl -u oapw -f

2. KONFLIKTE & TEST-SZENARIEN (Andere Programme, die auf die DAC-Hardware zugreifen wollen / müssen)
Da der OAPW-Player sehr nah an der Hardware arbeitet (miniaudio ALSA Push-Mode), sperrt er die Soundkarte exklusiv für sich. 

- Navidrome: Kein Problem. Navidrome streamt Musik ins Netzwerk und nutzt die lokale Soundkarte nicht.
  
3. AIRPLAY (Shairport Sync)
AirPlay läuft über den separaten Dienst "shairport-sync". Dieser ist so konfiguriert (in /etc/shairport-sync.conf), 
dass er den Audiostream direkt in die Pipe /tmp/oapw_stream schiebt, anstatt ihn an die Soundkarte zu senden. 
=========================================================

=========================================================
ROADMAP & ZUKÜNFTIGE ERWEITERUNGEN
=========================================================

1. Deployment auf anderen Hardware-Setups
Das OAPW-System ist vollständig hardwareunabhängig. Das exakt gleiche Setup (Hintergrunddienst + Shairport Sync + Named Pipe) ist z.B.
für den Einsatz auf einem Raspberry Pi 4 geeignet und getestet worden, der an einen Yamaha Vollverstärker mit passiven 
Canton Lautsprechern in einem anderen Raum angeschlossen ist. Da die Audio-Engine externe USB-DACs nahtlos unterstützt, 
kann der Code dort identisch kompiliert und als "oapw.service" im Hintergrund betrieben werden.

2. Nativer Port für macOS (Eigenständige App OAPW_ Mac)
Da die RACEDspEngine in reinem, standardisiertem C und C++17 geschrieben und strikt von der Audio-Ausgabe und dem Webserver getrennt ist, 
lässt sich der Rechenkern 1:1 z.B. auch auf Apple-Hardware übertragen. Ein zukünftiges Ziel ist eine gekapselte, eigenständige Desktop-App 
unter macOS Tahoe 26.2. Diese ersetzt den Webserver durch eine native Benutzeroberfläche (z. B. via SwiftUI) und gestattet die Verarbeitung 
systemweiter Audio-Streams direkt auf dem Mac. Als Beta-Version in GitHub verfügbar.
=========================================================
