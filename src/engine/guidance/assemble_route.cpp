#include "engine/guidance/assemble_route.hpp"

#include <numeric>

namespace osrm
{
namespace engine
{
namespace guidance
{

template <typename GeometryT>
RouteData assembleRoute(const std::vector<RouteLeg<GeometryT>> &route_legs)
{
    auto distance = std::accumulate(
        route_legs.begin(), route_legs.end(), 0., [](const double sum, const auto &leg) {
            return sum + leg.distance;
        });
    auto duration = std::accumulate(
        route_legs.begin(), route_legs.end(), 0., [](const double sum, const auto &leg) {
            return sum + leg.duration;
        });
    auto weight = std::accumulate(
        route_legs.begin(), route_legs.end(), 0., [](const double sum, const auto &leg) {
            return sum + leg.weight;
        });

    return RouteData{distance, duration, weight};
}

} // namespace guidance
} // namespace engine
} // namespace osrm
