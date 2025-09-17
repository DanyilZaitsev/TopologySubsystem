#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility> // for using pair in vector
#include "Graph.h"

Graph::Graph (const std::vector<Edge>& edges) : edges(edges) {
        
    // counting the vertices of a graph
    int vertices = 0;
    for (const auto& e : edges) {
        vertices = std::max(vertices, std::max(e.from, e.to));
    }
    V = vertices + 1; // final value of vertices

    // assigning vector dimension row_ptr
    row_ptr.resize(V + 1, 0);

    // counting the number of edges in a particular node of a graph
    for (const auto& e : edges) {
        row_ptr[e.from + 1]++; // 
    }

    // accumulate - where the list of edges begins 
    for (int i = 1; i <= V; ++i) {
        row_ptr[i] += row_ptr[i - 1];
    }

    // fill vector col_idx
    col_idx.resize(edges.size());
    edge_ids.resize(edges.size());
    std::vector<int> current_pos = row_ptr;  // буфер текущих позиций

    for (const auto& e : edges) {
        int pos = current_pos[e.from]++;
        col_idx[pos] = e.to;
        edge_ids[pos] = e.id;
    }
}

// get neighbors of a node
std::vector<int> Graph::getNeighbors(int v) const {
    std::vector<int> neighbors;
    for (int i = row_ptr[v]; i < row_ptr[v + 1]; ++i) {
        neighbors.push_back(col_idx[i]);
    }
    return neighbors;
}

// get neighbors with edge ids of a node
std::vector<std::pair<int,int>> Graph::getNeighborsWithEdgeIds(int v) const {
    std::vector<std::pair<int,int>> neighbors;
    for (int i = row_ptr[v]; i < row_ptr[v + 1]; ++i) {
        neighbors.emplace_back(col_idx[i], edge_ids[i]);
    }
    return neighbors;
}

// print the graph
void Graph::printGraph() const {
    for (int v = 0; v < V; ++v) {
        std::cout << "Verice " << v << ": ";
        for (int i = row_ptr[v]; i < row_ptr[v + 1]; ++i) {
            std::cout << col_idx[i] << "(id=" << edge_ids[i] << ") ";
        }
        std::cout << "\n";
    }
}

// return count of vertices
int Graph::numVertices() const {
    return V;
}

// get neighbors with attributes of a node
std::vector<std::pair<int, std::unordered_map<std::string,double>>>
Graph::getNeighborsWithAttributes(int v) const {
    std::vector<std::pair<int, std::unordered_map<std::string,double>>> neighbors;
    for (int i = row_ptr[v]; i < row_ptr[v + 1]; ++i) {
        neighbors.emplace_back(col_idx[i], edges[i].attributes);
    }
    return neighbors;
}

double Graph::getAttribute(int branchId, const std::string& attrName) const {
    for (const auto& e : edges) {
        if (e.id == branchId) {
            auto it = e.attributes.find(attrName);
            if (it != e.attributes.end()) {
                return it->second;
            } else {
                throw std::runtime_error("Attribute '" + attrName + "' not found in branch " + std::to_string(branchId));
            }
        }
    }
    throw std::runtime_error("Branch with id=" + std::to_string(branchId) + " not found");
}

const std::vector<Edge>& Graph::getEdges() const {
    return edges;
}

const std::vector<int>& Graph::getRowPtr() const { return row_ptr; }
const std::vector<int>& Graph::getColIdx() const { return col_idx; }
const std::vector<int>& Graph::getEdgeIds() const { return edge_ids; }

