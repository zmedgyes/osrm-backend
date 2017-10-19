#ifndef OSRM_ENGINE_API_MATCH_RESULT
#define OSRM_ENGINE_API_MATCH_RESULT

#include "engine/api/tracepoint.hpp"
#include "engine/api/matching.hpp"
#include "util/typedefs.hpp"

#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{
struct MatchResult
{
    std::vector<Tracepoint> sources;
    std::vector<Matching> destinations;
};
}
}
}
#endif
