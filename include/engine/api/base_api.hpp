#ifndef ENGINE_API_BASE_API_HPP
#define ENGINE_API_BASE_API_HPP

#include "engine/api/base_parameters.hpp"
#include "engine/datafacade/datafacade_base.hpp"

#include "engine/api/json_factory.hpp"
#include "engine/hint.hpp"

#include <boost/assert.hpp>
#include <boost/range/algorithm/transform.hpp>

#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{

class BaseAPI
{
  public:
    BaseAPI(const datafacade::BaseDataFacade &facade_, const BaseParameters &parameters_)
        : facade(facade_), parameters(parameters_)
    {
    }

    virtual std::vector<Waypoint> MakeWaypoints(const std::vector<PhantomNode> &phantoms,
                                                const std::vector<std::size_t> &indices) const
    {
        std::vector<Waypoint> waypoints;
        waypoints.reserve(indices.size());
        boost::range::transform(
            indices, std::back_inserter(waypoints), [this, phantoms](const std::size_t idx) {
                BOOST_ASSERT(idx < phantoms.size());
                auto name =
                    facade.GetNameForID(facade.GetNameIndex(phantoms[idx].forward_segment_id.id));
                return Waypoint{0, name.data(), phantoms[idx].location};
            });
        return waypoints;
    }

    virtual std::vector<Waypoint> MakeWaypoints(const std::vector<PhantomNode> &phantoms) const
    {
        std::vector<Waypoint> waypoints;
        waypoints.reserve(phantoms.size());
        BOOST_ASSERT(phantoms.size() == parameters.coordinates.size());

        boost::range::transform(
            phantoms, std::back_inserter(waypoints), [this](const PhantomNode &phantom) {
                auto name = facade.GetNameForID(facade.GetNameIndex(phantom.forward_segment_id.id));
                return Waypoint{0, name.data(), phantom.location};
            });
        return waypoints;
    }

    // TODO remove
    util::json::Array MakeJSONWaypoints(const std::vector<PhantomNodes> &segment_end_coordinates) const
    {
        BOOST_ASSERT(parameters.coordinates.size() > 0);
        BOOST_ASSERT(parameters.coordinates.size() == segment_end_coordinates.size() + 1);

        util::json::Array waypoints;
        waypoints.values.resize(parameters.coordinates.size());
        waypoints.values[0] = MakeWaypoint(segment_end_coordinates.front().source_phantom);

        auto out_iter = std::next(waypoints.values.begin());
        boost::range::transform(
            segment_end_coordinates, out_iter, [this](const PhantomNodes &phantom_pair) {
                return MakeWaypoint(phantom_pair.target_phantom);
            });
        return waypoints;
    }

    // FIXME: gcc 4.9 does not like MakeWaypoints to be protected
    // protected:
    util::json::Object MakeWaypoint(const PhantomNode &phantom) const
    {
        if (parameters.generate_hints)
        {
            // TODO: check forward/reverse
            return json::makeWaypoint(
                phantom.location,
                facade.GetNameForID(facade.GetNameIndex(phantom.forward_segment_id.id)).to_string(),
                Hint{phantom, facade.GetCheckSum()});
        }
        else
        {
            // TODO: check forward/reverse
            return json::makeWaypoint(
                phantom.location,
                facade.GetNameForID(facade.GetNameIndex(phantom.forward_segment_id.id))
                    .to_string());
        }
    }

    const datafacade::BaseDataFacade &facade;
    const BaseParameters &parameters;
};

} // ns api
} // ns engine
} // ns osrm

#endif
