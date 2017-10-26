#ifndef ENGINE_GUIDANCE_ANNOTATION_HPP
#define ENGINE_GUIDANCE_ANNOTATION_HPP

#include "util/typedefs.hpp"

namespace osrm
{
namespace engine
{
namespace guidance
{
// Per-coordinate metadata
struct Annotation
{
    double distance; // distance in meters

    // Total duration of a segment, in seconds, NOT including
    // the turn penalty if the segment preceeds a turn
    double duration;
    double weight; // weight value, NOT including the turn weight

    DatasourceID datasource;
};
}
}
}

#endif
