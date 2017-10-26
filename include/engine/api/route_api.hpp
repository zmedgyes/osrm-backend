#ifndef ENGINE_API_ROUTE_HPP
#define ENGINE_API_ROUTE_HPP

#include "engine/api/base_api.hpp"
#include "engine/api/json_factory.hpp"
#include "engine/api/route_parameters.hpp"
#include "engine/api/route_result.hpp"

#include "engine/datafacade/datafacade_base.hpp"

#include "engine/guidance/assemble_geometry.hpp"
#include "engine/guidance/assemble_leg.hpp"
#include "engine/guidance/assemble_overview.hpp"
#include "engine/guidance/assemble_route.hpp"
#include "engine/guidance/assemble_steps.hpp"
#include "engine/guidance/collapse_turns.hpp"
#include "engine/guidance/lane_processing.hpp"
#include "engine/polyline_compressor.hpp"
#include "engine/guidance/post_processing.hpp"
#include "engine/guidance/verbosity_reduction.hpp"

#include "engine/internal_route_result.hpp"

#include "util/coordinate.hpp"
#include "util/integer_range.hpp"
#include "util/json_util.hpp"
#include "util/typedefs.hpp"

#include <iterator>
#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{

class RouteAPI : public BaseAPI
{
  public:
    RouteAPI(const datafacade::BaseDataFacade &facade_, const RouteParameters &parameters_)
        : BaseAPI(facade_, parameters_), parameters(parameters_)
    {
    }

    RouteResult MakeResponse(const InternalManyRoutesResult &raw_routes) const
    {
        BOOST_ASSERT(!raw_routes.routes.empty());

        RouteResult route_result;

        for (const auto &route : raw_routes.routes)
        {
            if (!route.is_valid())
                continue;

            route_result.routes.push_back(MakeRoute(route.segment_end_coordinates,
                                                    route.unpacked_path_segments,
                                                    route.source_traversed_in_reverse,
                                                    route.target_traversed_in_reverse));
        }
        route_result.waypoints =
            BaseAPI::MakeWaypoints(raw_routes.routes[0].segment_end_coordinates);
        route_result.routes = std::move(routes);
    }

  protected:
    template <typename GetFn, typename AT>
    auto GetAnnotations(const guidance::LegGeometry &leg, GetFn Get) const
    {
        std::vector<AT> annotations_store;
        annotations_store.reserve(leg.annotations.size());
        std::for_each(leg.annotations.begin(),
                      leg.annotations.end(),
                      [Get, &annotations_store](const auto &step) {
                          annotations_store.push_back(Get(step));
                      });
        return annotations_store;
    }

    ApiRoute MakeRoute(const std::vector<PhantomNodes> &segment_end_coordinates,
                       const std::vector<std::vector<PathData>> &unpacked_path_segments,
                       const std::vector<bool> &source_traversed_in_reverse,
                       const std::vector<bool> &target_traversed_in_reverse) const
    {
        // Assemble RouteLegs, populate with data
        if (parameters.geometries == RouteParameters::GeometriesType::GeoJSON)
        {
            std::vector<api::RouteLeg<util::json::Object>> legs;
        } else
        {
            std::vector<api::RouteLeg<std::string>> legs;
        }
        std::vector<guidance::LegGeometry> leg_geometries;
        auto number_of_legs = segment_end_coordinates.size();
        legs.reserve(number_of_legs);
        leg_geometries.reserve(number_of_legs);

        // populate data for each leg of the route
        for (auto idx : util::irange<std::size_t>(0UL, number_of_legs))
        {
            const auto &phantoms = segment_end_coordinates[idx];
            const auto &path_data = unpacked_path_segments[idx];

            const bool reversed_source = source_traversed_in_reverse[idx];
            const bool reversed_target = target_traversed_in_reverse[idx];

            // assemble geometry and metadata of leg, including annotation data
            auto leg_geometry = guidance::assembleLegGeometry(BaseAPI::facade,
                                                           path_data,
                                                           phantoms.source_phantom,
                                                           phantoms.target_phantom,
                                                           reversed_source,
                                                           reversed_target);
            auto leg = guidance::assembleLeg(facade,
                                             path_data,
                                             leg_geometry,
                                             phantoms.source_phantom,
                                             phantoms.target_phantom,
                                             reversed_target,
                                             parameters.steps);

            if (parameters.steps)
            {
                auto steps = guidance::assembleSteps(BaseAPI::facade,
                                                     path_data,
                                                     leg_geometry,
                                                     phantoms.source_phantom,
                                                     phantoms.target_phantom,
                                                     reversed_source,
                                                     reversed_target);

                /* Perform step-based post-processing.
                 *
                 * By running post-processing on the route steps of a single leg at a time, we
                 * cannot count the correct exit for roundabouts.
                 * We can only emit the exit nr/intersections up to/starting at a part of the leg.
                 * If a roundabout is not terminated in a leg, we will end up with a
                 * enter-roundabout and exit-roundabout-nr where the exit nr is out of sync with the
                 *previous enter.
                 *
                 *         | S |
                 *         *   *
                 *  ----*        * ----
                 *                  T
                 *  ----*        * ----
                 *       V *   *
                 *         |   |
                 *         |   |
                 *
                 * Coming from S via V to T, we end up with the legs S->V and V->T. V-T will say to
                 *take
                 * the second exit, even though counting from S it would be the third.
                 * For S, we only emit `roundabout` without an exit number, showing that we enter a
                 *roundabout
                 * to find a via point.
                 * The same exit will be emitted, though, if we should start routing at S, making
                 * the overall response consistent.
                 *
                 * ⚠ CAUTION: order of post-processing steps is important
                 *    - handleRoundabouts must be called before collapseTurnInstructions that
                 *      expects post-processed roundabouts
                 */

                guidance::trimShortSegments(steps, leg_geometry);
                leg.steps = guidance::handleRoundabouts(std::move(steps));
                leg.steps = guidance::collapseTurnInstructions(std::move(leg.steps));
                leg.steps = guidance::anticipateLaneChange(std::move(leg.steps));
                leg.steps = guidance::buildIntersections(std::move(leg.steps));
                leg.steps = guidance::suppressShortNameSegments(std::move(leg.steps));
                leg.steps = guidance::assignRelativeLocations(std::move(leg.steps),
                                                              leg_geometry,
                                                              phantoms.source_phantom,
                                                              phantoms.target_phantom);
                leg_geometry = guidance::resyncGeometry(std::move(leg_geometry), leg.steps);
            }

            leg_geometries.push_back(std::move(leg_geometry));
            legs.push_back(std::move(leg));
        }

        // populate duration, distance and weight fields of route
        auto route_data = guidance::assembleRoute(legs);

        // Handle creating overview geometry
        if (parameters.overview != RouteParameters::OverviewType::False)
        {
            const auto use_simplification =
                parameters.overview == RouteParameters::OverviewType::Simplified;
            BOOST_ASSERT(use_simplification ||
                         parameters.overview == RouteParameters::OverviewType::Full);

            auto overview = guidance::assembleOverview(leg_geometries, use_simplification);
            if (parameters.geometries == RouteParameters::GeometriesType::Polyline)
            {
                auto overview = makePolyline<100000>(overview.begin(), overview.end());
            }
            if (parameters.geometries == RouteParameters::GeometriesType::Polyline6)
            {
                auto overview = makePolyline<1000000>(overview.begin(), overview.end());
            }
            BOOST_ASSERT(parameters.geometries == RouteParameters::GeometriesType::GeoJSON);
            auto json_overview = json::makeGeoJSONGeometry(overview.begin(), overview.end());
        }
        else
        {
            auto overview = nullptr;
        }

        // Create geometry for each route step
        if (parameters.geometries == RouteParameters::GeometriesType::GeoJSON)
        {
                std::vector<util::json::Object> step_geometries;
        } else
        {
                std::vector<std::string> step_geometries;
        }
        for (const auto idx : util::irange<std::size_t>(0UL, legs.size()))
        {
            auto &leg_geometry = leg_geometries[idx];

            auto handlePolyline = [this, &leg_geometry](const guidance::RouteStep &step,
                                                        std::size_t precision) {
                return makePolyline<precision>(leg_geometry.locations.begin() + step.geometry_begin,
                                               leg_geometry.locations.begin() + step.geometry_end);
            };
            auto handleGeoJSON = [this, &leg_geometry](const guidance::RouteStep &step) {
                return json::makeGeoJSONGeometry(
                    leg_geometry.locations.begin() + step.geometry_begin,
                    leg_geometry.locations.begin() + step.geometry_end);
            };
            if (parameters.geometries == RouteParameters::GeometriesType::Polyline ||
                parameters.geometries == RouteParameters::GeometriesType::Polyline6)
            {
                std::size_t Precision =
                    parameters.geometries == RouteParameters::GeometriesType::Polyline ? 10000
                                                                                       : 1000000;
                std::transform(legs[idx].steps.begin(),
                               legs[idx].steps.end(),
                               std::back_inserter(step_geometries),
                               handlePolyline(legs[idx], Precision));
            }
            if (parameters.geometries == RouteParameters::GeometriesType::GeoJSON)
            {
                std::transform(legs[idx].steps.begin(),
                               legs[idx].steps.end(),
                               std::back_inserter(step_geometries),
                               handleGeoJSON(legs[idx]));
            }
        }

        // To maintain support for uses of the old default constructors, we check
        // if annotations property was set manually after default construction
        auto requested_annotations = parameters.annotations_type;
        if ((parameters.annotations == true) &&
            (parameters.annotations_type == RouteParameters::AnnotationsType::None))
        {
            requested_annotations = RouteParameters::AnnotationsType::All;
        }

        if (requested_annotations != RouteParameters::AnnotationsType::None)
        {
            for (const auto idx : util::irange<std::size_t>(0UL, leg_geometries.size()))
            {
                auto &leg_geometry = leg_geometries[idx];
                ApiAnnotation annotation;

                // AnnotationsType uses bit flags, & operator checks if a property is set
                if (parameters.annotations_type & RouteParameters::AnnotationsType::Speed)
                {
                    annotation.speed = GetAnnotations<double>(
                        leg_geometry, [](const guidance::LegGeometry::Annotation &anno) {
                            return val = std::round(anno.distance / anno.duration * 10.) / 10.;
                        });
                }

                if (requested_annotations & RouteParameters::AnnotationsType::Duration)
                {
                    annotation.duration = GetAnnotations<double>(
                        leg_geometry, [](const guidance::LegGeometry::Annotation &anno) {
                            return anno.duration;
                        });
                }
                if (requested_annotations & RouteParameters::AnnotationsType::Distance)
                {
                    annotation.distance = GetAnnotations<double>(
                        leg_geometry, [](const guidance::LegGeometry::Annotation &anno) {
                            return anno.distance;
                        });
                }
                if (requested_annotations & RouteParameters::AnnotationsType::Weight)
                {
                    annotation.weight = GetAnnotations<double>(
                        leg_geometry,
                        [](const guidance::LegGeometry::Annotation &anno) { return anno.weight; });
                }
                if (requested_annotations & RouteParameters::AnnotationsType::Datasources)
                {
                    annotation.datasources = GetAnnotations<DatasourceID>(
                        leg_geometry, [](const guidance::LegGeometry::Annotation &anno) {
                            return anno.datasource;
                        });
                }
                if (requested_annotations & RouteParameters::AnnotationsType::Nodes)
                {
                    std::vector<std::uint64_t> nodes;
                    nodes.reserve(leg_geometry.osm_node_ids.size());
                    std::for_each(leg_geometry.osm_node_ids.begin(),
                                  leg_geometry.osm_node_ids.end(),
                                  [this, &nodes](const OSMNodeID &node_id) {
                                      nodes.push_back(static_cast<std::uint64_t>(node_id));
                                  });
                    annotation.nodes = std::move(nodes);
                }

                legs[idx].annotations.push_back(std::move(annotation));
            }
        }

        auto result = ApiRoute(route_data.distance,
                               route_data.duration,
                               route_data.weight,
                               facade.GetWeightName(),
                               overview,
                               std::move(legs));

        return result;
    }

    const RouteParameters &parameters;
};

} // ns api
} // ns engine
} // ns osrm

#endif
