#include <iostream>
#include <vector>
#include <jack/jack.h>

// Globale Zeiger für die JACK-Ports
jack_port_t* inputPortLeft   = nullptr;
jack_port_t* inputPortRight  = nullptr;
jack_port_t* outputPortLeft  = nullptr;
jack_port_t* outputPortRight = nullptr;

jack_client_t* client = nullptr;

// Das ist der Echtzeit-Callback! 
// Wird von PipeWire/JACK für jeden Audio-Puffer (z.B. 128 oder 256 Samples) aufgerufen.
int processAudio(jack_nframes_t nframes, void* arg)
{
    (void)arg; // Unbenutzt

    // Puffer-Zeiger für Eingänge holen
    auto* inL = static_cast<float*>(jack_port_get_buffer(inputPortLeft, nframes));
    auto* inR = static_cast<float*>(jack_port_get_buffer(inputPortRight, nframes));

    // Puffer-Zeiger für Ausgänge holen
    auto* outL = static_cast<float*>(jack_port_get_buffer(outputPortLeft, nframes));
    auto* outR = static_cast<float*>(jack_port_get_buffer(outputPortRight, nframes));

    // Etappe 2: Reiner Pass-Through (1:1 Durchschleifen)
    for (jack_nframes_t i = 0; i < nframes; ++i)
    {
        outL[i] = inL[i];
        outR[i] = inR[i];
    }

    return 0; // 0 bedeutet: Erfolgreich verarbeitet
}

void jackShutdown(void* arg)
{
    (void)arg;
    std::cout << "[OAPW] JACK Server hat die Verbindung getrennt." << std::endl;
}

int main()
{
    jack_status_t status;
    
    // 1. Verbindung zu PipeWire/JACK herstellen
    client = jack_client_open("OAPW_Ambiophonics", JackNullOption, &status);
    if (!client)
    {
        std::cerr << "[OAPW Error] Konnte keinen JACK/PipeWire Client öffnen. Status: " 
                  << status << std::endl;
        return 1;
    }

    std::cout << "[OAPW] Verbindung zu PipeWire erfolgreich hergestellt." << std::endl;
    std::cout << "[OAPW] Sample Rate: " << jack_get_sample_rate(client) << " Hz" << std::endl;
    std::cout << "[OAPW] Puffer-Größe: " << jack_get_buffer_size(client) << " Samples" << std::endl;

    // 2. Process-Callback registrieren
    jack_set_process_callback(client, processAudio, nullptr);
    jack_on_shutdown(client, jackShutdown, nullptr);

    // 3. Audio-Ports anlegen
    inputPortLeft = jack_port_register(
        client, "in_l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    inputPortRight = jack_port_register(
        client, "in_r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    outputPortLeft = jack_port_register(
        client, "out_l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    outputPortRight = jack_port_register(
        client, "out_r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    if (!inputPortLeft || !inputPortRight || !outputPortLeft || !outputPortRight)
    {
        std::cerr << "[OAPW Error] Konnte Audio-Ports nicht registrieren!" << std::endl;
        jack_client_close(client);
        return 1;
    }

    // 4. Client aktivieren
    if (jack_activate(client))
    {
        std::cerr << "[OAPW Error] Client konnte nicht aktiviert werden!" << std::endl;
        jack_client_close(client);
        return 1;
    }

    std::cout << "\n>>> OAPW Audio Node läuft im Pass-Through Modus <<<" << std::endl;
    std::cout << "Drücke [ENTER], um den Server zu stoppen..." << std::endl;

    // Warten, bis der Nutzer ENTER drückt
    std::cin.get();

    // 5. Aufräumen
    jack_client_close(client);
    std::cout << "[OAPW] Client beendet." << std::endl;

    return 0;
}
