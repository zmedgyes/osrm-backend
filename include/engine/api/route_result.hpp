#ifndef OSRM_ENGINE_API_ROUTE_RESULT_HPP
#define OSRM_ENGINE_API_ROUTE_RESULT_HPP

#include "engine/api/waypoint.hpp"
#include "engine/api/route.hpp"

#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{

template <typename GeometryT>
struct RouteResult
{
    std::vector<Waypoint> waypoints;
    std::vector<ApiRoute<GeometryT>> routes;
};
}
}
}

#endif
