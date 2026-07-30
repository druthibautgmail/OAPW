=========================================================
OAPW - Hintergrunddienst & Audio-Routing (Stand: Mitte 2026)
=========================================================

1. DER HINTERGRUNDDIENST (oapw.service)
Unser C++ OAPW-Player läuft permanent als unsichtbarer Hintergrunddienst auf dem Raspberry Pi 5. Er horcht auf die Named Pipe (/tmp/oapw_stream) und gibt das Signal echtzeitnah über ALSA an den DAC aus. Das Web-Interface ist immer über Port 8080 erreichbar.

WICHTIGE BEFEHLE FÜR DAS TERMINAL:
- Player manuell stoppen:   sudo systemctl stop oapw
- Player manuell starten:   sudo systemctl start oapw
- Player neu starten:       sudo systemctl restart oapw
- Status/Fehler anzeigen:   sudo systemctl status oapw
- Live-Logbuch ansehen:     journalctl -u oapw -f

2. KONFLIKTE & TEST-SZENARIEN (Java Programme)
Da der OAPW-Player sehr nah an der Hardware arbeitet (miniaudio ALSA Push-Mode), sperrt er die Soundkarte exklusiv für sich. 

- Navidrome: Kein Problem. Navidrome streamt Musik ins Netzwerk und nutzt die lokale Soundkarte nicht.
- Stephan Hottos Java-Programme: Konflikt! Wenn du die alten Java-RACE-Programme für Referenztests starten willst, blockiert der C++ Player die Audioausgabe. 
  
LÖSUNG FÜR JAVA-TESTS:
Vor dem Starten der Java-Programme musst du den C++ Player zwingend schlafen legen:
1. "sudo systemctl stop oapw" ausführen.
2. Java-Tests durchführen.
3. Danach "sudo systemctl start oapw" ausführen, damit AirPlay wieder funktioniert.

3. AIRPLAY (Shairport Sync)
AirPlay läuft über den separaten Dienst "shairport-sync". Dieser ist so konfiguriert (in /etc/shairport-sync.conf), dass er den Audiostream direkt in die Pipe /tmp/oapw_stream schiebt, anstatt ihn an die Soundkarte zu senden. 
=========================================================

=========================================================
ROADMAP & ZUKÜNFTIGE ERWEITERUNGEN
=========================================================

1. Multi-Room Deployment (Spa-Bereich)
Das OAPW-System ist vollständig hardwareunabhängig. Das exakt gleiche Setup (Hintergrunddienst + Shairport Sync + Named Pipe) ist für den Einsatz auf einem Raspberry Pi 4 vorgesehen, der an einen Yamaha Vollverstärker mit passiven Canton Lautsprechern im Spa angeschlossen ist. Da die Audio-Engine externe USB-DACs nahtlos unterstützt, kann der Code dort identisch kompiliert und als "oapw.service" im Hintergrund betrieben werden.

2. Nativer Port für macOS (Eigenständige App)
Da die RACEDspEngine in reinem, standardisiertem C++17 geschrieben und strikt von der Audio-Ausgabe und dem Webserver getrennt ist, lässt sich der Rechenkern 1:1 auf Apple-Hardware übertragen. Ein zukünftiges Ziel ist eine gekapselte, eigenständige Desktop-App unter macOS Tahoe 26.2. Diese würde den Webserver durch eine native Benutzeroberfläche (z. B. via SwiftUI oder Objective-C++) ersetzen und systemweite Audio-Streams direkt auf dem Mac verarbeiten.
=========================================================
