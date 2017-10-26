#ifndef OSRM_ENGINE_API_ANNOTATION_HPP
#define OSRM_ENGINE_API_ANNOTATION_HPP

#include <vector>

#include "util/typedefs.hpp"

namespace osrm
{
namespace engine
{
namespace api
{
struct Annotation
{
    std::vector<double> distance;
    std::vector<double> duration;
    std::vector<DatasourceID> datasources;
    std::vector<double> nodes;
    std::vector<double> weight;
    std::vector<double> speed;
};
}
}
}

#endif
