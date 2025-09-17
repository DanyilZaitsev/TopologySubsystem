#ifndef DECOMPOSITION_H
#define DECOMPOSITION_H

#include "Graph.cpp"
#include <vector>
#include <utility>

class Decomposition {
    private:
        const Graph& graph;

        std::vector<bool> visited;

        std::vector<std::tuple<int, int, int>> baum;
        std::vector<std::tuple<int, int, int>> antibaum;

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
