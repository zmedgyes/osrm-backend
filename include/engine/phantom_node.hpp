#ifndef PHANTOM_NODES_H
#define PHANTOM_NODES_H

#include "extractor/travel_mode.hpp"
#include "util/typedefs.hpp"

#include "osrm/coordinate.hpp"

#include <iostream>
#include <utility>
#include <vector>

namespace osrm
{
namespace engine
{

struct PhantomNode
{
    struct ComponentType
    {
        uint32_t id : 31;
        bool is_tiny : 1;
    };
// bit-fields are broken on Windows
#ifndef _MSC_VER
    static_assert(sizeof(ComponentType) == 4, "ComponentType needs to be 4 bytes big");
#endif

    PhantomNode()
        : forward_node_id(SPECIAL_NODEID), reverse_node_id(SPECIAL_NODEID),
          name_id(std::numeric_limits<unsigned>::max()), forward_weight(INVALID_EDGE_WEIGHT),
          reverse_weight(INVALID_EDGE_WEIGHT), forward_offset(0), reverse_offset(0),
          forward_packed_geometry_id(SPECIAL_EDGEID), reverse_packed_geometry_id(SPECIAL_EDGEID),
          component{INVALID_COMPONENTID, false}, fwd_segment_position(0),
          forward_travel_mode(TRAVEL_MODE_INACCESSIBLE),
          backward_travel_mode(TRAVEL_MODE_INACCESSIBLE)
    {
    }

    bool IsBidirected() const
    {
        return (forward_node_id != SPECIAL_NODEID) && (reverse_node_id != SPECIAL_NODEID);
    }

    bool IsCompressed() const { return (forward_offset != 0) || (reverse_offset != 0); }

    bool IsValid(const unsigned number_of_nodes) const
    {
        return location.IsValid() &&
               ((forward_node_id < number_of_nodes) || (reverse_node_id < number_of_nodes)) &&
               ((forward_weight != INVALID_EDGE_WEIGHT) ||
                (reverse_weight != INVALID_EDGE_WEIGHT)) &&
               (component.id != INVALID_COMPONENTID) && (name_id != INVALID_NAMEID);
    }

    bool IsValid() const { return location.IsValid() && (name_id != INVALID_NAMEID); }

    bool operator==(const PhantomNode &other) const { return location == other.location; }

    template <class OtherT>
    explicit PhantomNode(const OtherT &other,
                         int forward_offset_,
                         int reverse_offset_,
                         const util::Coordinate foot_point)
    {
        forward_segment_id = other.forward_segment_id;
        reverse_segment_id = other.reverse_segment_id;
        name_id = other.name_id;

        forward_offset = forward_offset_;
        reverse_offset = reverse_offset_;

        forward_geometry_id = other.forward_geometry_id;
        reverse_geometry_id = other.reverse_geometry_id;

        component.id = other.component.id;
        component.is_tiny = other.component.is_tiny;

        location = foot_point;
        fwd_segment_position = other.fwd_segment_position;

        travel_mode = other.travel_mode;
    }

    NodeID forward_segment_id = SPECIAL_NODEID;
    NodeID reverse_segment_id = SPECIAL_NODEID;
    unsigned name_id = INVALID_NAMEID;
    // offset from the snapped coordinate to the forward point of the compressed geometry
    // u -----x-----> v
    //        ^ snapped coordinate
    // |>>>>>>| forward offset
    int forward_offset = INVALID_EDGE_WEIGHT;
    // offset from the snapped coordinate to the reverse point of the compressed geometry
    // u -----x-----> v
    //        ^ snapped coordinate
    //        |<<<<<| reverse offset
    int reverse_offset = INVALID_EDGE_WEIGHT;
    unsigned forward_geometry_id = INVALID_GEOMETRYID;
    unsigned reverse_geometry_id = INVALID_GEOMETRYID;
    ComponentType component = {INVALID_COMPONENTID, false};
    util::Coordinate location;
    unsigned short fwd_segment_position;
    // note 4 bits would suffice for each,
    // but the saved byte would be padding anyway
    PackedTravelMode travel_mode = {TRAVEL_MODE_INACCESSIBLE, TRAVEL_MODE_INACCESSIBLE};
};

#ifndef _MSC_VER
static_assert(sizeof(PhantomNode) == 52, "PhantomNode has more padding then expected");
#endif

using PhantomNodePair = std::pair<PhantomNode, PhantomNode>;

struct PhantomNodeWithDistance
{
    PhantomNode phantom_node;
    double distance;
};

struct PhantomNodes
{
    PhantomNode source_phantom;
    PhantomNode target_phantom;
};
}
}

#endif // PHANTOM_NODES_H
