#ifndef OSRM_ENGINE_API_STEP_MANEUVER_HPP
#define OSRM_ENGINE_API_STEP_MANEUVER_HPP

#include "util/coordinate.hpp"

#include <string>

namespace osrm
{
namespace engine
{
namespace api
{
struct StepManeuver
{
    util::Coordinate location;
    short bearing_before;
    short bearing_after;
    std::string type;
    std::string modifier;
    unsigned exit;
}
}
}
}
#endif
