#include "oapw/core/race_processor.hpp"
#include "oapw/core/recursive_correction_engine.hpp"

namespace oapw::core
{

void RaceProcessor::configure(
    const config::RaceConfiguration& configuration)
{
    // Konfiguration speichern
    configuration_ = configuration;
    configured_ = true;

    setGainMatrix(configuration.gainMatrix);

    // Filterkoeffizienten aus neuer Konfiguration anwenden
    updateFilterCoefficients();

    // Falls prepare() noch nicht aufgerufen wurde,
    // wird die Delay-Konfiguration später nachgeholt.
    if (sampleRate_ <= 0.0)
    {
        return;
    }

    const double delaySamples =
        configuration.crossDelaySeconds * sampleRate_;

    setDelaySamples(
        delaySamples,
        delaySamples);
}

void RaceProcessor::setGainMatrix(
    const GainMatrix& gains)
{
    gainMatrix_ = gains;
}

void RaceProcessor::updateFilterCoefficients()
{
    float alpha = configuration_.headShadowAlpha;
    float g     = configuration_.attenuationFactor;

    // Entspricht einem One-Pole-Tiefpass: y[n] = g*alpha*x[n] + (1-alpha)*y[n-1]
    leftFilter_.setCoefficients(g * alpha, 0.0f, -(1.0f - alpha));
    rightFilter_.setCoefficients(g * alpha, 0.0f, -(1.0f - alpha));
}

void RaceProcessor::prepare(
    double sampleRate,
    std::size_t maximumDelaySamples)
{
    recursiveCorrectionEngine_.prepare(sampleRate);

    sampleRate_ = sampleRate;

    leftDelay_.prepare(
        sampleRate,
        maximumDelaySamples);

    rightDelay_.prepare(
        sampleRate,
        maximumDelaySamples);

    leftFilter_.prepare(sampleRate);
    rightFilter_.prepare(sampleRate);

    leftFilter_.reset();
    rightFilter_.reset();

    // Dynamische Filterkoeffizienten aus der Konfiguration zuweisen
    updateFilterCoefficients();

    // Historie des Rekursionszustands auf Null setzen
    lastOutputLeft_ = 0.0f;
    lastOutputRight_ = 0.0f;

    // Falls configure() bereits aufgerufen wurde,
    // können jetzt die Delays korrekt gesetzt werden.
    if (configured_)
    {
        const double delaySamples =
            configuration_.crossDelaySeconds * sampleRate_;

        setDelaySamples(
            delaySamples,
            delaySamples);
    }
}

void RaceProcessor::setDelaySamples(
    double leftDelaySamples,
    double rightDelaySamples)
{
    leftDelay_.setDelay(leftDelaySamples);
    rightDelay_.setDelay(rightDelaySamples);
}

void RaceProcessor::process(
    float inputLeft,
    float inputRight,
    float& outputLeft,
    float& outputRight)
{
    // Variables for storing the historic cross-talk elements extracted from the delays
    float delayedCrosstalkLeft = 0.0f;
    float delayedCrosstalkRight = 0.0f;

    // 1. Die vergangenheitsbasierten Signale aus den Delays holen.
    leftDelay_.process(&lastOutputLeft_, &delayedCrosstalkLeft, 1);
    rightDelay_.process(&lastOutputRight_, &delayedCrosstalkRight, 1);

    // 2. Frequenzabhängige Kopfschattendämpfung per Tiefpass auf den Crosstalk anwenden.
    float filteredCrosstalkRight = leftFilter_.process(delayedCrosstalkRight);
    float filteredCrosstalkLeft  = rightFilter_.process(delayedCrosstalkLeft);

    // 3. Rekursive Subtraktion nach Ralph Glasgal (Kreuzweise Auslöschung)
    float currentLeftOut  = inputLeft  - filteredCrosstalkRight;
    float currentRightOut = inputRight - filteredCrosstalkLeft;

    // 4. Ausgänge zuweisen
    outputLeft  = currentLeftOut;
    outputRight = currentRightOut;

    // 5. Zustand für den nächsten Sample-Schritt sichern
    lastOutputLeft_  = currentLeftOut;
    lastOutputRight_ = currentRightOut;
}

} // namespace oapw::core
