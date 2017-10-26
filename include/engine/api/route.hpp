#ifndef OSRM_ENGINE_API_ROUTE_HPP
#define OSRM_ENGINE_API_ROUTE_HPP

#include "engine/api/geometry.hpp"
#include "engine/api/waypoint.hpp"
#include "engine/guidance/route_data.hpp"
#include "engine/guidance/route_leg.hpp"

namespace osrm
{
namespace engine
{
namespace api
{
template <typename GeometryT> struct ApiRoute : guidance::RouteData
{
    const char *weight_name;
    GeometryT geometry;
    std::vector<guidance::RouteLeg<GeometryT>> legs;

    ApiRoute(double distance_,
             double duration_,
             double weight_,
             const char *weight_name_,
             GeometryT geometry_,
             std::vector<guidance::RouteLeg<GeometryT>> legs_)
        : RouteData(distance_, duration_, weight_), weight_name(weight_name_), geometry(geometry_),
          legs(legs_)
    {
    }
};
}
}
}

#endif
