#ifndef OSRM_ENGINE_API_GEOMETRY_HPP
#define OSRM_ENGINE_API_GEOMETRY_HPP

#include "util/json_container.hpp"
#include <string>

namespace osrm
{
namespace engine
{
namespace api
{
struct PolylineGeometry
{
    std::string geometry;
};

struct GeoJSONGeometry
{
    util::json::Object geometry;
};
}
}
}

#endif
