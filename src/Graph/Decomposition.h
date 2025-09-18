#ifndef DECOMPOSITION_H
#define DECOMPOSITION_H

#include "Graph.cpp"
#include <vector>
#include <utility>
#include <unordered_set>

class Decomposition {
    private:
        const Graph& graph;

        std::vector<bool> visited;

        std::vector<std::tuple<int, int, int>> baum;
        std::vector<std::tuple<int, int, int>> antibaum;

        // Track which edge ids have been categorized to avoid duplicates and losses
        std::unordered_set<int> treeEdgeIds;
        std::unordered_set<int> antiEdgeIds;

        void dfs(int v);

    public:
        // Constructor
        Decomposition(const Graph& graph);

        // get 
        const std::vector<std::tuple<int, int, int>>& getBaum() const;
        const std::vector<std::tuple<int, int, int>>& getAntibaum() const;

        // build baum and antibaum
        void build();

        void print() const;
};

#endif
