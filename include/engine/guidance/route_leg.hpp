#ifndef ROUTE_LEG_HPP
#define ROUTE_LEG_HPP

#include "engine/api/annotation.hpp"
#include "engine/api/route_step.hpp"

#include <boost/optional.hpp>

#include <string>
#include <vector>

namespace osrm
{
namespace engine
{
namespace guidance
{

template <typename GeometryT>
struct RouteLeg
{
    double distance;
    double duration;
    double weight;
    std::string summary;
    std::vector<api::RouteStep<GeometryT>> steps;
    api::ApiAnnotation annotations;
};
}
}
}

#endif
