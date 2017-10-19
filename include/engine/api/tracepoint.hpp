#ifndef OSRM_ENGINE_API_TRACEPOINT_HPP
#define OSRM_ENGINE_API_TRACEPOINT_HPP

#include "util/coordinate.hpp"
#include "engine/api/waypoint.hpp"

namespace osrm
{
namespace engine
{
namespace api
{
struct Tracepoint : Waypoint
{
    double distance;
    const char *name;
    util::Coordinate location;
    std::int32_t matchings_index;
    std::int32_t waypoint_index;
    std::int32_t alternatives_count;
};
}
}
}

#endif
