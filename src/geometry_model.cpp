#include "geometry_model.h"
#include <cmath>

namespace oapw::core
{

GeometryModel::GeometryModel(const GeometryParameters& parameters)
    : parameters_(parameters)
{
}

GeometryResult GeometryModel::calculate() const
{
    GeometryResult result;

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

Point2D GeometryModel::listener() const
{
    return {
        0.0,
        parameters_.listeningDistance
    };
}
}
