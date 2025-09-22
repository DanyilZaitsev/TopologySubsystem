#include "IncidenceMatrix.h"

IncidenceMatrix::IncidenceMatrix(const Graph& graph) {
    int V = graph.numVertices();
    int E = graph.getEdgeIds().size();

    matrix.assign(V, std::vector<int>(E, 0));

    for (int u = 0; u < V; ++u) {
        for (int i = graph.getRowPtr()[u]; i < graph.getRowPtr()[u + 1]; i++) {
            int v = graph.getColIdx()[i];
            int eid = graph.getEdgeIds()[i]; 

            int col = eid - 1; // если id ребра начинается с 1
            if (col < 0 || col >= E) continue;

            matrix[u][col] = -1; 
            matrix[v][col] = +1; 
        }
    }
}

const std::vector<std::vector<int>>& IncidenceMatrix::getMatrix() const {
    return matrix;
}

void IncidenceMatrix::print() const {
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

void IncidenceMatrix::validateAndPrintSummary() const {
    int V = matrix.size();
    int E = matrix.empty() ? 0 : matrix[0].size();

    std::cout << "\nValidation of incidence matrix:\n";
    for (int e = 0; e < E; ++e) {
        int from = -1, to = -1;
        for (int v = 0; v < V; ++v) {
            if (matrix[v][e] == -1) from = v;
            if (matrix[v][e] == 1) to = v;
        }

        std::cout << "Edge " << (e + 1) << ": ";
        if (from != -1 && to != -1) {
            std::cout << from << " -> " << to << " ✅\n";
        } else {
            std::cout << "⚠️ Invalid column (not exactly one -1 and one +1)\n";
        }
    }
}
