#ifndef EDGE_BASED_NODE_HPP
#define EDGE_BASED_NODE_HPP

#include "extractor/travel_mode.hpp"
#include "util/typedefs.hpp"

#include <boost/assert.hpp>

#include "osrm/coordinate.hpp"

#include <limits>

namespace osrm
{
namespace extractor
{

/// This is what util::StaticRTree serialized and stores on disk
/// It is generated in EdgeBasedGraphFactory.
struct EdgeBasedNode
{
    EdgeBasedNode()
        : forward_segment_id(SPECIAL_NODEID), reverse_segment_id(SPECIAL_NODEID),
          u(SPECIAL_NODEID), v(SPECIAL_NODEID), name_id(0),
          forward_geometry_id(SPECIAL_EDGEID), reverse_geometry_id(SPECIAL_EDGEID),
          component{INVALID_COMPONENTID, false},
          fwd_segment_position(std::numeric_limits<unsigned short>::max()),
          travel_mode{TRAVEL_MODE_INACCESSIBLE, TRAVEL_MODE_INACCESSIBLE}
    {
    }

    explicit EdgeBasedNode(NodeID forward_segment_id,
                           NodeID reverse_segment_id,
                           NodeID u,
                           NodeID v,
                           unsigned name_id,
                           unsigned forward_geometry_id_,
                           unsigned reverse_geometry_id_,
                           bool is_tiny_component,
                           unsigned component_id,
                           unsigned short fwd_segment_position,
                           TravelMode forward_travel_mode,
                           TravelMode backward_travel_mode)
        : forward_segment_id(forward_segment_id),
          reverse_segment_id(reverse_segment_id), u(u), v(v), name_id(name_id),
          forward_geometry_id(forward_geometry_id_),
          reverse_geometry_id(reverse_geometry_id_),
          component{component_id, is_tiny_component}, fwd_segment_position(fwd_segment_position),
          travel_mode{forward_travel_mode, backward_travel_mode}
    {
        BOOST_ASSERT((forward_edge_based_node_id != SPECIAL_NODEID) ||
                     (reverse_edge_based_node_id != SPECIAL_NODEID));
    }

    NodeID forward_segment_id; // needed for edge-expanded graph
    NodeID reverse_segment_id; // needed for edge-expanded graph
    NodeID u;                          // indices into the coordinates array
    NodeID v;                          // indices into the coordinates array
    unsigned name_id;                  // id of the edge name

    unsigned forward_geometry_id;
    unsigned reverse_geometry_id;
    struct
    {
        unsigned id : 31;
        bool is_tiny : 1;
    } component;
    unsigned short fwd_segment_position; // segment id in a compressed geometry
    PackedTravelMode travel_mode;
};
}
}

#endif // EDGE_BASED_NODE_HPP
