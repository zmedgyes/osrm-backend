#ifndef ENGINE_GUIDANCE_ASSEMBLE_ROUTE_HPP
#define ENGINE_GUIDANCE_ASSEMBLE_ROUTE_HPP

#include "engine/guidance/route_data.hpp"
#include "engine/guidance/route_leg.hpp"

#include <vector>

namespace osrm
{
namespace engine
{
namespace guidance
{

template <typename GeometryT>
RouteData assembleRoute(const std::vector<RouteLeg<GeometryT>> &route_legs);

} // namespace guidance
} // namespace engine
} // namespace osrm

#endif
