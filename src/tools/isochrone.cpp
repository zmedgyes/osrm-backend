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

namespace osrm
{
namespace tools
{

struct SimpleEdgeData
{
    SimpleEdgeData() : weight(INVALID_EDGE_WEIGHT) {}
    SimpleEdgeData(unsigned weight) : weight(weight) {}
    unsigned weight;
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

        if (input_edge.forward)
        {
            graph_edge_list.emplace_back(input_edge.source, input_edge.target, input_edge.weight);
        }
        if (input_edge.backward)
        {
            graph_edge_list.emplace_back(input_edge.target, input_edge.source, input_edge.weight);
        }
    }

    return number_of_nodes;
}
}
}

int main(int argc, char *argv[])
{
    std::vector<osrm::extractor::QueryNode> coordinate_list;
    osrm::util::LogPolicy::GetInstance().Unmute();

    // enable logging
    if (argc < 2)
    {
        osrm::util::SimpleLogger().Write(logWARNING) << "usage:\n" << argv[0] << " <filename.osrm>";
        return EXIT_FAILURE;
    }

    std::vector<osrm::tools::SimpleEdge> graph_edge_list;
    std::string base(argv[1]);
    auto number_of_nodes = osrm::tools::loadGraph(base, coordinate_list, graph_edge_list);

    tbb::parallel_sort(graph_edge_list.begin(), graph_edge_list.end());
    const auto graph =
        std::make_shared<osrm::tools::SimpleGraph>(number_of_nodes, graph_edge_list);
    graph_edge_list.clear();
    graph_edge_list.shrink_to_fit();


    // Set up the datafacade for querying
    std::unordered_map<std::string, boost::filesystem::path> server_paths;
    server_paths["base"] = base;
    osrm::util::populate_base_path(server_paths);
    osrm::engine::datafacade::InternalDataFacade<osrm::contractor::QueryEdge::EdgeData> datafacade(server_paths);

    //
    // Find nearest point within 1000m
    //
    auto phantomnodes = datafacade.NearestPhantomNodesInRange({static_cast<int>(37.7600 * osrm::COORDINATE_PRECISION), static_cast<int>(-122.4554 * osrm::COORDINATE_PRECISION)}, 1000);
    auto phantom = phantomnodes[0];
    NodeID source = 0;
    if (datafacade.EdgeIsCompressed(phantom.phantom_node.forward_node_id))
    {
        std::vector<NodeID> forward_id_vector;
        datafacade.GetUncompressedGeometry(phantom.phantom_node.forward_node_id, forward_id_vector);
        source = forward_id_vector[phantom.phantom_node.fwd_segment_position];
        // TODO: initialize the weight using what's remaining on the snapped edge
        //       unfortunately, that data isn't readily available
        // startWeight += phantom.forward_weight;  // WRONG: weight is the *start* of the segment, not the end that we want
    }
    //
    // Perform search
    //
    //
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
    const int MAX = 500000;

    std::unordered_set<NodeID> edgepoints;

    {

        boost::timer::auto_cpu_timer t;
    // Standard Dijkstra search, terminating when path length > MAX
    while (!heap.Empty()) {
        const NodeID source = heap.DeleteMin();
        const std::int32_t distance = heap.GetKey(source);

        for (const auto current_edge : graph->GetAdjacentEdgeRange(source)) {
            const auto target = graph->GetTarget(current_edge);
            if (target != SPECIAL_NODEID) {
                const auto data = graph->GetEdgeData(current_edge);
                int to_distance = distance + data.weight;
                if (to_distance > MAX) {
                    edgepoints.insert(source);
                }
                else if (!heap.WasInserted(target)) {
                    heap.Insert(target, to_distance, source);
                }
                else if (to_distance < heap.GetKey(target)) {
                    heap.GetData(target).parent = source;
                    heap.DecreaseKey(target, to_distance);
                }

            }
        }
    }

    std::cout << std::setprecision(8);
    std::cout << "{ \"type\": \"Feature\",\"properties\":{},\"geometry\":{\"type\":\"Point\", \"coordinates\":[" 
        << -122.4554 << "," << 37.7600 << "]}},\n";
    // Now, dump all the edge points
    for (const auto edgepoint : edgepoints)
    {
        std::cout << "{ \"type\": \"Feature\",\"properties\":{},\"geometry\":{\"type\":\"Point\", \"coordinates\":[" 
            << coordinate_list[edgepoint].lon / osrm::COORDINATE_PRECISION << "," << coordinate_list[edgepoint].lat / osrm::COORDINATE_PRECISION << "]}},\n";
    }

    }

    return EXIT_SUCCESS;
}
