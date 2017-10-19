#ifndef OSRM_ENGINE_API_ANNOTATION_HPP
#define OSRM_ENGINE_API_ANNOTATION_HPP

#include <vector>

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
    std::vector<std::uint8_t> datasources;
    std::vector<double> nodes;
    std::vector<double> weight;
};
}
}
}

#endif
