#ifndef OSRM_ENGINE_API_ROUTE_HPP
#define OSRM_ENGINE_API_ROUTE_HPP

#include "boost/variant.hpp"

#include "engine/api/route_leg.hpp"

namespace osrm
{
namespace engine
{
namespace api
{

struct Route
{
    double distance;
    double duration;
    double weight;
    boost::variant<std::string, util::json::Object> geometry;
    const char *weight_name;
    std::vector<RouteLeg> legs;
};
}
}
}

#endif
