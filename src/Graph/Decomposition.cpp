#include "Decomposition.h"
#include <algorithm>

Decomposition::Decomposition(const Graph& g) : graph(g), visited(g.numVertices(), false) {}


void Decomposition::dfs(int v) {
    visited[v] = true;

    for ( int i = graph.getRowPtr()[v]; i < graph.getRowPtr()[v + 1]; i++ ) {
        int u = graph.getColIdx()[i];
        int eid = graph.getEdgeIds()[i];

        if (!visited[u]) {
            baum.emplace_back(v, u, eid);
            dfs(u);
        } else if (v < u) {
            antibaum.emplace_back(v, u, eid);
        }
    }
}

void Decomposition::build() {
    baum.clear();
    antibaum.clear();
    std::fill(visited.begin(), visited.end(), false);

    dfs(0);
}

const std::vector<std::tuple<int, int, int>>& Decomposition::getBaum() const {
    return baum;
}

const std::vector<std::tuple<int, int, int>>& Decomposition::getAntibaum() const {
    return antibaum;
}

void Decomposition::print() const {
    std::cout << "Baum:\n";
    for (auto [u, v, id] : baum) {
        std::cout << u << " - " << v << " (id=" << id << ")\n";
    }

    std::cout << "\nAnti-Baum:\n";
    for (auto [u, v, id] : antibaum) {
        std::cout << u << " - " << v << " (id=" << id << ")\n";
    }
}
