#pragma once

namespace oapw::core
{

struct Point2D
{
    double x {};
    double y {};

    double distanceTo(const Point2D& other) const;
};

struct Listener
{
    Point2D head;
    Point2D leftEar;
    Point2D rightEar;
};

struct GeometryParameters
{
    double speakerDistance {};
    double listeningDistance {};
    double earDistance {0.18};

    double speedOfSound {343.2};
};

struct GeometryResult
{
    Point2D leftSpeaker;
    Point2D rightSpeaker;

    Listener listener;

    double dLL {};
    double dLR {};
    double dRL {};
    double dRR {};

double deltaDistance {};

double delaySeconds {};

double delayMilliseconds {};

double delayMicroseconds {};

double delaySamples44k1 {};

double delaySamples48k {};

double delaySamples96k {};

double attenuation {};
};

class GeometryModel
{
/*public:

    explicit GeometryModel(const GeometryParameters& parameters);

    GeometryResult calculate() const;

    Point2D leftSpeaker() const;
    Point2D rightSpeaker() const;

    Point2D listener() const;
*/

public:

    explicit GeometryModel(const GeometryParameters& parameters);

    GeometryResult calculate() const;

    Point2D leftSpeaker() const;

    Point2D rightSpeaker() const;

    Listener listener() const;

private:

    GeometryParameters parameters_;
};

}
