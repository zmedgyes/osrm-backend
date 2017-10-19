#ifndef OSRM_ENGINE_API_ROUTE_HPP
#define OSRM_ENGINE_API_ROUTE_HPP

#include "util/geometry.hpp"
#include "engine/api/route_leg.hpp"
#include "engine/api/waypoint.hpp"

namespace osrm
{
namespace engine
{
namespace api
{
template <typename GeometryT> struct Route
{
    double distance;
    float duration;
    GeometryT geometry;
    double weight;
    const char *weight_name;
    std::vector<RouteLeg> legs;
};
}
}
}

#endif
