#include "Decomposition.h"
#include <algorithm>
#include <queue>

Decomposition::Decomposition(const Graph& g) : graph(g), visited(g.numVertices(), false) {}


void Decomposition::dfs(int v) {
    std::queue<int> queue;
    visited[v] = true;
    queue.push(v);

    while (!queue.empty()) {
        int cur = queue.front();
        queue.pop();

        for (int i = graph.getRowPtr()[cur]; i < graph.getRowPtr()[cur + 1]; i++) {
            int u = graph.getColIdx()[i];
            int eid = graph.getEdgeIds()[i];

            if (!visited[u]) {
                visited[u] = true;
                if (treeEdgeIds.insert(eid).second) {
                    baum.emplace_back(cur, u, eid);
                }
                queue.push(u);
            } else {
                // Non-tree edge; include it if it wasn't already captured as a tree edge
                if (!treeEdgeIds.count(eid) && antiEdgeIds.insert(eid).second) {
                    antibaum.emplace_back(cur, u, eid);
                }
            }
        }
    }
}

void Decomposition::build() {
    baum.clear();
    antibaum.clear();
    treeEdgeIds.clear();
    antiEdgeIds.clear();
    std::fill(visited.begin(), visited.end(), false);

    // Traverse all components using BFS-based dfs
    for (int v = 0; v < (int)visited.size(); ++v) {
        if (!visited[v]) {
            dfs(v);
        }
    }
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
