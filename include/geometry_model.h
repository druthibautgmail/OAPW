#pragma once

namespace oapw::core
{

struct Point2D
{
    double x {};
    double y {};
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
    double dLL {};
    double dLR {};
    double dRL {};
    double dRR {};

    double delaySeconds {};

    double delaySamples44k1 {};
    double delaySamples48k {};

    double attenuation {};
};

class GeometryModel
{
public:

    explicit GeometryModel(const GeometryParameters& parameters);

    GeometryResult calculate() const;

private:

    GeometryParameters parameters_;
};

}
