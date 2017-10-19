#ifndef OSRM_ENGINE_API_ROUTE_STEP_HPP
#define OSRM_ENGINE_API_ROUTE_STEP_HPP

#include "engine/api/geometry.hpp"
#include "engine/api/intersection.hpp"
#include "engine/api/step_maneuver.hpp"

#include <string>
#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{
struct RouteStep
{
    double distance;
    double duration;
    GeometryT geometry;
    double weight;
    std::string name;
    std::string ref;
    std::string pronunciation;
    std::string destinations;
    std::string exits;
    std::string mode;
    StepManeuver maneuver;
    std::vector<Intersection> intersection;
    std::string rotary_name;
    std::string rotary_pronunciation;
};
}
}
}

#endif
