#ifndef OSRM_ENGINE_API_INTERSECTION_HPP
#define OSRM_ENGINE_API_INTERSECTION_HPP

#include "util/coordinate.hpp"
#include "util/guidance/turn_lanes.hpp"

#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{
struct Intersection
{
    util::Coordinate location;
    std::vector<double> bearings;
    std::size_t in;
    std::size_t out;
    std::vector<bool> entry;
    std::vector<std::string> classes;
    std::vector<LaneTuple> lanes; // this might change?
};
}
}
}

#endif
