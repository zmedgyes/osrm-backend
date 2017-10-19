#ifndef OSRM_ENGINE_API_ROUTE_LEG_HPP
#define OSRM_ENGINE_API_ROUTE_LEG_HPP

#include "engine/api/annotation.hpp"
#include "engine/api/route_step.hpp"
#include "util/geometry.hpp"

#include <string>
#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{
struct RouteLeg
{
    double distance;
    double duration;
    GeometryT geometry;
    double weight;
    std::string summary;
    std::vector<RouteStep> steps;
    Annotation annotation;
};
}
}
}

#endif
