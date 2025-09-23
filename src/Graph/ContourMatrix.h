#ifndef CONTOUR_MATRIX_H
#define CONTOUR_MATRIX_H

#include <vector>
#include <unordered_map>
#include "Graph.h"
#include "Decomposition.h"

using SparseCycle = std::unordered_map<int,int>;

class ContourMatrix {
private:
    const Graph& graph;
    const Decomposition& dec; //  baum - antibaum

    // tree adjacency built from baum: node -> vector<{neighbor, edgeId}>
    std::vector<std::vector<std::pair<int,int>>> treeAdj;

    // parent info for tree nodes
    std::vector<int> parent;      // parent[node] in tree forest (-1 if root)
    std::vector<int> parentEdge;  // edgeId used to connect node to parent
    std::vector<int> depth;

    // sparse cycles: one row per anti-edge
    std::vector<SparseCycle> cycles;
    std::vector<int> cycleAntiEdgeIds; // aligns with cycles indices
    int minEdgeId = 0, maxEdgeId = -1;

    void buildTreeAdjFromBaum();
    void prepareParentDepth(); // DFS over forest to fill parent, depth, parentEdge
    // get path edges from node u up to node v (inclusive endpoints), returning vector of (edgeId, fromNode, toNode)
    // We will produce path from u -> ... -> v (direction of traversal)
    std::vector<std::tuple<int,int,int>> getPathEdgesNodes(int u, int v) const;

public:
    ContourMatrix(const Graph& g, const Decomposition& d);

    // build cycles from antibaum
    void build();

    // get sparse cycles
    const std::vector<SparseCycle>& getSparseCycles() const { return cycles; }

    // optional: return dense matrix V x E (may be heavy)
    std::vector<std::vector<int>> toDenseMatrix() const;

    // pretty print (sparse)
    void printSparse() const;

    // pretty print dense
    void printDense() const;

    // validation: each cycle row must be a closed walk (zero divergence),
    // contain exactly one anti-edge (the one used to generate it), and all
    // other edges must belong to the Baum
    bool validate() const;
};

#endif
