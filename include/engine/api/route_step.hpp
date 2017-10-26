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
template <typename GeometryT> struct RouteStep
{
    double duration; // duration in seconds
    double distance; // distance in meters
    double weight;   // weight value
    GeometryT geometry;
    std::string name;
    std::string ref;
    std::string pronunciation;
    std::string destinations;
    std::string exits;
    std::string mode;
    StepManeuver maneuver;
    std::vector<Intersection> intersections;
    std::string rotary_name;
    std::string rotary_pronunciation;
};
}
}
}

#endif
