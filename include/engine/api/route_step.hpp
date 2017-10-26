#ifndef OSRM_API_ROUTE_STEP_HPP
#define OSRM_API_ROUTE_STEP_HPP

#include "boost/variant.hpp"

#include "engine/api/intersection.hpp"
#include "engine/api/step_maneuver.hpp"
#include "util/coordinate.hpp"
#include "util/json_container.hpp"

#include <vector>
#include <string>

namespace osrm
{
namespace engine
{
namespace api
{
struct RouteStep
{
    std::string name;
    std::string ref;
    std::string pronunciation;
    std::string destinations;
    std::string exits;
    std::string rotary_name;
    std::string rotary_pronunciation;
    double duration; // duration in seconds
    double distance; // distance in meters
    double weight;   // weight value
    std::string mode;
    StepManeuver maneuver;
    boost::variant<std::string, util::json::Object> geometry;
    std::vector<Intersection> intersections;
}
}
}
}

#endif
