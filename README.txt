=========================================================
OAPW - System-Administration & Audio-Routing (Version 11)
=========================================================

Dieses Dokument beschreibt die systemseitige Konfiguration und den laufenden Betrieb der OAPW-Engine auf dem Raspberry Pi 5 unter Linux.

1. DER HINTERGRUNDDIENST (oapw.service)
---------------------------------------------------------
Die C++ OAPW-Player Engine v11 (Psychoakustik-Update) läuft permanent als autarker Hintergrunddienst. Der Dienst horcht kontinuierlich auf die Named Pipe (/tmp/oapw_stream), verarbeitet das Signal durch die phasenlineare Biquad-Filterkette und gibt es exklusiv und echtzeitnah über ALSA an den DAC aus. Das integrierte Web-Interface zur Live-Steuerung und Auto-Kalibrierung ist dauerhaft über Port 8080 erreichbar.

Wichtige Terminal-Befehle für die Dienst-Verwaltung:
* Player manuell stoppen:   sudo systemctl stop oapw
* Player manuell starten:   sudo systemctl start oapw
* Player neu starten:       sudo systemctl restart oapw
* Status/Fehler anzeigen:   sudo systemctl status oapw
* Live-Logbuch ansehen:     journalctl -u oapw -f


2. AIRPLAY-INTEGRATION (Shairport Sync)
---------------------------------------------------------
Das systemweite AirPlay-Routing wird über den separaten Dienst "shairport-sync" abgewickelt. Dieser ist in der Konfigurationsdatei (/etc/shairport-sync.conf) so eingerichtet, dass er den decodierten Audiostream nicht direkt an die Hardware-Soundkarte sendet, sondern ihn als Raw-Audio nahtlos in die Named Pipe (/tmp/oapw_stream) schiebt, von wo die OAPW-Engine ihn abgreift.


3. EXKLUSIVER HARDWARE-ZUGRIFF & TEST-SZENARIEN (Java)
---------------------------------------------------------
Da die v11 C++ Engine für minimale Latenzen extrem nah an der Hardware arbeitet (miniaudio ALSA Push-Mode), sperrt sie den Audio-DAC exklusiv für sich.

* Unkritische Dienste (z. B. Navidrome): Kein Konflikt. Navidrome streamt lediglich Musik ins Netzwerk und greift nicht auf den lokalen ALSA-Treiber zu.
* Konflikt mit anderen Programmen: Wenn Programme gestartet werden, die die ALSA-Schnittstelle ansprechen, kollidieren diese mit dem blockierten ALSA-Treiber des C++ Players. Konflikte können vermieden werden, wenn nach dem entweder/oder Prinzip gearbeitet wird, das heißt: Entweder oapw oder das andere Programm, das auf ALSA zugreift laufen lassen.

Lösungsansatzh:
Vor dem Ausführen der anderen Programme muss der C++ Hintergrunddienst zwingend freigegeben werden:
1. Terminal-Befehl ausführen: sudo systemctl stop oapw
2. Tests auf der nun freien Soundkarte durchführen.
3. Terminal-Befehl ausführen: sudo systemctl start oapw (AirPlay und das C++ Web-Interface sind danach sofort wieder aktiv).

=========================================================
Hinweis zur Entwicklung:
Die langfristige Zukunftsplanung (Nativer macOS Tahoe 26.2 Port, Multi-Room-Deployment mit anderer Stereo-Hardware) sowie die architektonische Dokumentation des C++ Codes sind in der Datei README.md auf GitHub hinterlegt.
=========================================================
