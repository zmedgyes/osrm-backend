#ifndef ENGINE_API_TABLE_HPP
#define ENGINE_API_TABLE_HPP

#include "engine/api/base_api.hpp"
#include "engine/api/json_factory.hpp"
#include "engine/api/table_parameters.hpp"
#include "engine/api/waypoint.hpp"

#include "engine/datafacade/datafacade_base.hpp"

#include "engine/guidance/assemble_geometry.hpp"
#include "engine/guidance/assemble_leg.hpp"
#include "engine/guidance/assemble_overview.hpp"
#include "engine/guidance/assemble_route.hpp"
#include "engine/guidance/assemble_steps.hpp"

#include "engine/internal_route_result.hpp"

#include "util/integer_range.hpp"

#include <boost/range/algorithm/transform.hpp>

#include <iterator>

namespace osrm
{
namespace engine
{
namespace api
{

class TableAPI final : public BaseAPI
{
  public:
    TableAPI(const datafacade::BaseDataFacade &facade_, const TableParameters &parameters_)
        : BaseAPI(facade_, parameters_), parameters(parameters_)
    {
    }

    virtual TableResult MakeResponse(const std::vector<EdgeWeight> &durations,
                                     const std::vector<PhantomNode> &phantoms) const
    {
        TableResult response;

        // symmetric case
        if (parameters.sources.empty())
        {
            response.sources = MakeWaypoints(phantoms);
        }
        else
        {
            response.sources = MakeWaypoints(phantoms, parameters.sources);
        }

        // symmetric case
        if (parameters.destinations.empty())
        {
            response.destinations = MakeWaypoints(phantoms);
        }
        else
        {
            response.destinations = MakeWaypoints(phantoms, parameters.destinations);
        }

        response.durations = std::move(durations);
        return response;
    }

  protected:
    const TableParameters &parameters;
};

} // ns api
} // ns engine
} // ns osrm

#endif
