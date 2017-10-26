#ifndef OSRM_API_LANE_HPP
#define OSRM_API_LANE_HPP

#include <vector>
#include <string>

namespace osrm
{
namespace engine
{
namespace api
{
struct Lane
{
    std::vector<std::string> indications;
    bool valid;
}
}
}
#endif
