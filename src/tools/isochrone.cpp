#include "util/typedefs.hpp"
#include "util/coordinate_calculation.hpp"
#include "util/dynamic_graph.hpp"
#include "util/static_graph.hpp"
#include "util/fingerprint.hpp"
#include "util/graph_loader.hpp"
#include "util/make_unique.hpp"
#include "util/osrm_exception.hpp"
#include "util/simple_logger.hpp"
#include "util/binary_heap.hpp"

#include "engine/datafacade/internal_datafacade.hpp"

#include "util/routed_options.hpp"

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

#include "osrm/coordinate.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <cstdlib>

namespace osrm
{
namespace tools
{

struct SimpleEdgeData
{
    SimpleEdgeData() : weight(INVALID_EDGE_WEIGHT), real(false) {}
    SimpleEdgeData(unsigned weight_, bool real_) : weight(weight_), real(real_) {}
    unsigned weight;
    bool real;
};

using SimpleGraph = util::StaticGraph<SimpleEdgeData>;
using SimpleEdge = SimpleGraph::InputEdge;

void deleteFileIfExists(const std::string &file_name)
{
    if (boost::filesystem::exists(file_name))
    {
        boost::filesystem::remove(file_name);
    }
}

std::size_t loadGraph(const std::string &path,
                      std::vector<extractor::QueryNode> &coordinate_list,
                      std::vector<SimpleEdge> &graph_edge_list)
{
    boost::timer::auto_cpu_timer t(std::cerr, "loadGraph: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
    std::ifstream input_stream(path, std::ifstream::in | std::ifstream::binary);
    if (!input_stream.is_open())
    {
        throw util::exception("Cannot open osrm file");
    }

    // load graph data
    std::vector<extractor::NodeBasedEdge> edge_list;
    std::vector<NodeID> traffic_light_node_list;
    std::vector<NodeID> barrier_node_list;

    auto number_of_nodes = util::loadNodesFromFile(input_stream, barrier_node_list,
                                                   traffic_light_node_list, coordinate_list);

    util::loadEdgesFromFile(input_stream, edge_list);

    traffic_light_node_list.clear();
    traffic_light_node_list.shrink_to_fit();

    // Building an node-based graph
    for (const auto &input_edge : edge_list)
    {
        if (input_edge.source == input_edge.target)
        {
            continue;
        }

        // forward edge
        graph_edge_list.emplace_back(input_edge.source, input_edge.target, input_edge.weight, input_edge.forward);
        // backward edge
        graph_edge_list.emplace_back(input_edge.target, input_edge.source, input_edge.weight, input_edge.backward);
    }

    return number_of_nodes;
}
}
}

int main(int argc, char *argv[])
{
    std::vector<osrm::extractor::QueryNode> coordinate_list;
    osrm::util::LogPolicy::GetInstance().Mute();

    // enable logging
    if (argc < 4)
    {
        osrm::util::SimpleLogger().Write(logWARNING) << "usage:\n" << argv[0] << " <points|border> <filename.osrm> <radius>";
        return EXIT_FAILURE;
    }

    std::vector<osrm::tools::SimpleEdge> graph_edge_list;
    std::string base(argv[2]);
    auto number_of_nodes = osrm::tools::loadGraph(base, coordinate_list, graph_edge_list);

    tbb::parallel_sort(graph_edge_list.begin(), graph_edge_list.end());
    const auto graph =
        std::make_shared<osrm::tools::SimpleGraph>(number_of_nodes, graph_edge_list);
    graph_edge_list.clear();
    graph_edge_list.shrink_to_fit();


    // Set up the datafacade for querying
    using Datafacade = osrm::engine::datafacade::InternalDataFacade<osrm::contractor::QueryEdge::EdgeData>;
    std::unique_ptr<Datafacade> datafacade;
    {
        boost::timer::auto_cpu_timer t(std::cerr, "datafacade: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
        std::unordered_map<std::string, boost::filesystem::path> server_paths;
        server_paths["base"] = base;
        osrm::util::populate_base_path(server_paths);
        datafacade = std::unique_ptr<Datafacade>(new Datafacade(server_paths));
    }

    const double LON = -122.44202613830566;
    const double LAT = 37.7785166660836;

    auto phantomnodes = datafacade->NearestPhantomNodes({static_cast<int>(LAT * osrm::COORDINATE_PRECISION), static_cast<int>(LON * osrm::COORDINATE_PRECISION)}, 1);

    std::sort(phantomnodes.begin(), phantomnodes.end(), [&](const osrm::engine::PhantomNodeWithDistance &a, const osrm::engine::PhantomNodeWithDistance &b){ return a.distance > b.distance; });
    auto phantom = phantomnodes[0];
    NodeID source = 0;

    if (datafacade->EdgeIsCompressed(phantom.phantom_node.forward_node_id))
    {
        std::vector<NodeID> forward_id_vector;
        datafacade->GetUncompressedGeometry(phantom.phantom_node.packed_geometry_id, forward_id_vector);
        source = forward_id_vector[phantom.phantom_node.fwd_segment_position];
    } else {
        source = phantom.phantom_node.packed_geometry_id;
    }
    //
    // Perform search
    //
    //
    //std::cerr << "Snapped to  " << coordinate_list[source].lon / osrm::COORDINATE_PRECISION << "," << coordinate_list[source].lat / osrm::COORDINATE_PRECISION << std::endl;
    struct HeapData
    {
        NodeID parent;
    /* explicit */ HeapData(NodeID p) : parent(p) {}
    };

    using QueryHeap =
        osrm::util::BinaryHeap<NodeID, NodeID, int, HeapData, osrm::util::UnorderedMapStorage<NodeID, int>>;

    QueryHeap heap(number_of_nodes);

    heap.Insert(source, -phantom.phantom_node.GetForwardWeightPlusOffset(), source);

    // value is in metres
    const int MAX = std::atoi(argv[3]);

    std::cout << std::setprecision(8);

    std::unordered_set<NodeID> edgepoints;
    std::unordered_set<NodeID> insidepoints;
    std::vector<NodeID> border;

    {
        boost::timer::auto_cpu_timer t(std::cerr, "dijkstra: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
        // Standard Dijkstra search, terminating when path length > MAX
        while (!heap.Empty()) {
            const NodeID source = heap.DeleteMin();
            const std::int32_t distance = heap.GetKey(source);
    
            for (const auto current_edge : graph->GetAdjacentEdgeRange(source)) {
                const auto target = graph->GetTarget(current_edge);
                if (target != SPECIAL_NODEID) {
                    const auto data = graph->GetEdgeData(current_edge);
                    if (data.real) {
                        int to_distance = distance + data.weight;
                        if (to_distance > MAX) {
                            edgepoints.insert(source);
                        }
                        else if (!heap.WasInserted(target)) {
                            heap.Insert(target, to_distance, source);
                            insidepoints.insert(source);
                        }
                        else if (to_distance < heap.GetKey(target)) {
                            heap.GetData(target).parent = source;
                            heap.DecreaseKey(target, to_distance);
                        }
                    }
                }
            }
        }
    }

    std::cout << "{\"type\":\"FeatureCollection\",\"features\":[\n";

    if (std::string(argv[1]) == "points") 
    {
        bool first = true;
        for (auto p : insidepoints)
        {
            if (!first) std::cout << ",\n";
            std::cout << "{\"type\":\"Feature\",\"properties\":{},\"geometry\":{\"type\":\"Point\",\"coordinates\":[";
            std::cout << coordinate_list[p].lon / osrm::COORDINATE_PRECISION << ", " << coordinate_list[p].lat / osrm::COORDINATE_PRECISION;
            std::cout << "]}}";
            first = false;
        }
        std::cout << "\n";
    }

    if (std::string(argv[1]) == "border")
    {
        boost::timer::auto_cpu_timer t(std::cerr, "border: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");

        NodeID startnode = SPECIAL_NODEID;

        // Find the north-west most edge node
        for (const auto node_id : insidepoints)
        {
            if (startnode == SPECIAL_NODEID) 
            {
                startnode = node_id;
            } 
            else 
            {
                if (coordinate_list[node_id].lon < coordinate_list[startnode].lon)  
                {
                    startnode = node_id;
                }
                else if (coordinate_list[node_id].lon == coordinate_list[startnode].lon
                        && coordinate_list[node_id].lat > coordinate_list[startnode].lat) 
                {
                    startnode = node_id;
                }

            }
        }
        //std::cerr << "Start node is " << startnode << " at " << coordinate_list[startnode].lon / osrm::COORDINATE_PRECISION << "," << coordinate_list[startnode].lat / osrm::COORDINATE_PRECISION << std::endl;


        NodeID node_u = startnode;
        border.push_back(node_u);

        // Find the outgoing edge with the angle closest to 180 (because we're at the west-most node,
        // there should be no edges with angles < 0 or > 180)
        NodeID node_v = SPECIAL_NODEID;
        for (const auto current_edge : graph->GetAdjacentEdgeRange(node_u)) {
            const auto target = graph->GetTarget(current_edge);
            if (target != SPECIAL_NODEID && insidepoints.find(target) != insidepoints.end()) {
                if (node_v == SPECIAL_NODEID) {
                    node_v = target;
                }
                else
                {
                    if (
                        osrm::util::coordinate_calculation::bearing(
                            coordinate_list[node_u],
                            coordinate_list[target]) >
                        osrm::util::coordinate_calculation::bearing(
                            coordinate_list[node_u],
                            coordinate_list[node_v]))
                    {
                        node_v = target;
                    }
                }
                BOOST_ASSERT( 0 <= osrm::util::coordinate_calculation::bearing(coordinate_list[node_u], coordinate_list[node_v]));
                BOOST_ASSERT( 180 >= osrm::util::coordinate_calculation::bearing(coordinate_list[node_u], coordinate_list[node_v]));
            }
        }
        //std::cerr << "Next node is " << node_v << " at " << coordinate_list[node_v].lon / osrm::COORDINATE_PRECISION << "," << coordinate_list[node_v].lat / osrm::COORDINATE_PRECISION << " at bearing " << osrm::util::coordinate_calculation::bearing( coordinate_list[node_u], coordinate_list[node_v]) << std::endl;
        //std::cout << "{\"type\":\"Feature\",\"properties\":{},\"geometry\":{\"type\":\"LineString\",\"coordinates\":";
        //std::cout << "[[" << coordinate_list[node_u].lon / osrm::COORDINATE_PRECISION << "," << coordinate_list[node_u].lat / osrm::COORDINATE_PRECISION << "],";
        //std::cout << "[" << coordinate_list[node_v].lon / osrm::COORDINATE_PRECISION << "," << coordinate_list[node_v].lat / osrm::COORDINATE_PRECISION << "]]}}";

        border.push_back(node_v);
        
        // Now, we're going to always turn right (relative to the last edge)
        // only onto nodes that are onthe inside point set
        NodeID firsttarget = node_v;
        while (true) {
            NodeID node_w = SPECIAL_NODEID;
            double best_angle = 361.0;
            for (const auto current_edge : graph->GetAdjacentEdgeRange(node_v)) {
                const auto target = graph->GetTarget(current_edge);
                if (target == SPECIAL_NODEID) continue;
                if (insidepoints.find(target) == insidepoints.end()) continue;

                auto angle = osrm::util::coordinate_calculation::computeAngle(
                                         coordinate_list[node_u],
                                         coordinate_list[node_v],
                                         coordinate_list[target]);
                if (node_w == SPECIAL_NODEID || angle > best_angle)
                {
                    node_w = target;
                    best_angle = angle;
                }
            }
            if (node_w == SPECIAL_NODEID) {
                std::cerr << "Couldn't find an edge inside" << std::endl;
                return EXIT_FAILURE;
            }

            if (firsttarget == node_w && startnode == node_v)
            {
                // Here, we've looped all the way around the outside and we've traversed
                // the first segment again.  Break!
                break;
            }
            border.push_back(node_w);

            node_u = node_v;
            node_v = node_w;
        }

        std::cout << "{\"type\":\"Feature\",\"properties\":{},\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[";
        bool first = true;
        for (auto n : border) {
            if (!first) std::cout << ",\n";
            std::cout << "[" << coordinate_list[n].lon / osrm::COORDINATE_PRECISION << "," << coordinate_list[n].lat / osrm::COORDINATE_PRECISION << "]";
            first = false;
        }
        std::cout << "]]}}";
        std::cout << "\n";
    }

    std::cout << "]}\n";

    return EXIT_SUCCESS;
}
