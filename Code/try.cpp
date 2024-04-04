#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;

// Define a struct to store vertex properties (optional)
struct VertexProperties {
  int id;
  // Add other properties if needed (e.g., color, label)
};

// Define an edge weight property map
typedef property_map<graph_traits<Graph>::edge_descriptor, double> EdgeWeightPropertyMap;

typedef adjacency_list<vecS, vecS, directedS, VertexProperties, EdgeWeightPropertyMap> Graph;


int main(int argc, char* argv[]) {
  // Create a graph
  Graph g;

  // Add some vertices
  int v1 = add_vertex(g);
  int v2 = add_vertex(g);
  int v3 = add_vertex(g);
  int v4 = add_vertex(g);

  // Add some edges with weights
  add_edge(v1, v2, 5.0, g);
  add_edge(v1, v3, 2.0, g);
  add_edge(v2, v4, 1.0, g);
  add_edge(v3, v4, 3.0, g);

  // Get the edge weight property map
  EdgeWeightPropertyMap weight_map = get(edge_weight, g);

  // Open an output file for the DOT representation
  fstream outputFileG1(argv[1], ios::out);

  // Write the graph to a DOT file
  write_graphviz(outputFileG1, g,
      // Access vertex ID directly using property map for clarity:
      make_label_writer(get(&VertexProperties::id, g)),
      // Convert edge weight to string using boost::lexical_cast:
      default_edge_label(transform(weight_map,
                                   make_transform_iterator(std::copy<double*>()),
                                   boost::lexical_cast<std::string, double>)));

  outputFileG1.close();

  return 0;
}
