#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <string>

struct Edge {
    int from;
    int to;
    int id;

    std::unordered_map<std::string, double> attributes;
};

class Graph {
  private:
    int V; // counting of vertices
    std::vector<int> col_idx; // counting of edges
    std::vector<int> row_ptr; // intervals
    std::vector<int> edge_ids; // edge ids aligned with col_idx

    std::vector<Edge> edges; // edges of the graph

  public:
    // Constructor
    Graph (const std::vector<Edge>& edges);

    // get neighbors of a node
    std::vector<int> getNeighbors(int v) const;

    // get neighbors with edge ids of a node
    std::vector<std::pair<int,int>> getNeighborsWithEdgeIds(int v) const;

    // print the graph
    void printGraph() const;

    // return count of vertices
    int numVertices() const;

    // get neighbors with attributes of a node
    std::vector<std::pair<int, std::unordered_map<std::string,double>>>
    getNeighborsWithAttributes(int v) const;

    double getAttribute(int branchId, const std::string& attrName) const;
};

#endif