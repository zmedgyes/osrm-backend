#ifndef OSRM_API_ROUTE_LEG_HPP
#define OSRM_API_ROUTE_LEG_HPP

#include "engine/api/route_step.hpp"
#include "engine/api/annotation.hpp"

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
    double weight;
    std::string summary;
    std::vector<RouteStep> steps;
    Annotation annotation;
}
}
}
}

#endif
