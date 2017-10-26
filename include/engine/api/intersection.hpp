#ifndef OSRM_API_INTERSECTION_HPP
#define OSRM_API_INTERSECTION_HPP

#include "util/coordinate.hpp"
#include "engine/api/lane.hpp"

#include <vector>
#include <string>

namespace osrm
{
namespace engine
{
namespace api
{
struct Intersection
{
    util::Coordinate location;
    std::vector<short> bearings;
    std::vector<bool> entry;
    std::size_t in;
    std::size_t out;
    std::vector<Lane> lanes;
    std::vector<std::string> classes;
}
}
}
}

#endif
