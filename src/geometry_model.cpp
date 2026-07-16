#include "oapw/core/geometry_model.hpp"
#include <cmath>

namespace oapw::core
{

double Point2D::distanceTo(const Point2D& other) const
{
    const double dx = other.x - x;
    const double dy = other.y - y;

    return std::sqrt(dx * dx + dy * dy);
}

GeometryModel::GeometryModel(const GeometryParameters& parameters)
    : parameters_(parameters)
{
}

GeometryResult GeometryModel::calculate() const
{
    GeometryResult result;

    result.leftSpeaker  = leftSpeaker();
    result.rightSpeaker = rightSpeaker();

    result.listener = listener();

    result.dLL =
        result.leftSpeaker.distanceTo(result.listener.leftEar);

    result.dLR =
        result.leftSpeaker.distanceTo(result.listener.rightEar);

    result.dRL =
        result.rightSpeaker.distanceTo(result.listener.leftEar);

    result.dRR =
        result.rightSpeaker.distanceTo(result.listener.rightEar);

// Laufwegdifferenz zwischen fernem und nahem Ohr
    result.deltaDistance = result.dLR - result.dLL;

// Laufzeit
    result.delaySeconds =
        result.deltaDistance / parameters_.speedOfSound;

    result.delayMilliseconds =
        result.delaySeconds * 1000.0;

    result.delayMicroseconds =
        result.delaySeconds * 1.0e6;

// Samples
    result.delaySamples44k1 =
        result.delaySeconds * 44100.0;

    result.delaySamples48k =
        result.delaySeconds * 48000.0;

    result.delaySamples96k =
        result.delaySeconds * 96000.0;

    return result;

}
Point2D GeometryModel::leftSpeaker() const
{
    return {
        -parameters_.speakerDistance / 2.0,
        0.0
    };
}

Point2D GeometryModel::rightSpeaker() const
{
    return {
         parameters_.speakerDistance / 2.0,
         0.0
    };
}

Listener GeometryModel::listener() const
{
    Listener l;

    l.head =
    {
        0.0,
        parameters_.listeningDistance
    };

    l.leftEar =
    {
        -parameters_.earDistance / 2.0,
        parameters_.listeningDistance
    };

    l.rightEar =
    {
         parameters_.earDistance / 2.0,
         parameters_.listeningDistance
    };

    return l;
}
}
