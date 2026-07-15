#include "geometry_model.h"

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

}
