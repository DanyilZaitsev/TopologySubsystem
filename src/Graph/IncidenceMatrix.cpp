#include "IncidenceMatrix.h"

#include <algorithm>
#include <stdexcept>


IncidenceMatrix::IncidenceMatrix(const Graph& graph) {
    int V = graph.numVertices();
    const auto& edges = graph.getEdges();

    if (edges.empty()) {
        matrix.clear();
        minEdgeId = 0;
        maxEdgeId = -1;
        return;
    }

    minEdgeId = edges.front().id;
    maxEdgeId = edges.front().id;
    for (const auto &e : edges) {
        minEdgeId = std::min(minEdgeId, e.id);
        maxEdgeId = std::max(maxEdgeId, e.id);
    }
    int Ecols = maxEdgeId - minEdgeId + 1;
    matrix.assign(V, std::vector<int>(Ecols, 0));

    // fill in the matrix from/to
    for (const auto &e : edges) {
        int col = e.id - minEdgeId;
        if (col < 0 || col >= Ecols) continue; // overinsurance
        if (e.from < 0 || e.from >= V || e.to < 0 || e.to >= V) {
            // omission or exclusion
            std::cerr << "Warning: edge id=" << e.id << " has invalid endpoints ("
                      << e.from << "," << e.to << ")\n";
            continue;
        }
        matrix[e.from][col] = -1; // from -> start
        matrix[e.to  ][col] = +1; // to   -> end
    }
}

void IncidenceMatrix::print() const {
    if (maxEdgeId < minEdgeId) {
        std::cout << "(empty incidence matrix)\n";
        return;
    }
    int Ecols = maxEdgeId - minEdgeId + 1;

    std::cout << "     ";
    for (int col = 0; col < Ecols; ++col) {
        std::cout << (minEdgeId + col) << (col + 1 < 10 ? "  " : " ");
    }
    std::cout << "\n";

    for (size_t r = 0; r < matrix.size(); ++r) {
        std::cout << r << ": ";
        for (int col = 0; col < Ecols; ++col) {
            int v = matrix[r][col];
            // tab
            if (v >= 0) std::cout << " ";
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
}

bool IncidenceMatrix::validate(bool printProblems) const {
    if (maxEdgeId < minEdgeId) return true; 

    int Ecols = maxEdgeId - minEdgeId + 1;
    bool ok = true;
    for (int col = 0; col < Ecols; ++col) {
        int cnt_minus1 = 0, cnt_plus1 = 0;
        for (size_t r = 0; r < matrix.size(); ++r) {
            if (matrix[r][col] == -1) ++cnt_minus1;
            if (matrix[r][col] == +1) ++cnt_plus1;
        }
        if (cnt_minus1 != 1 || cnt_plus1 != 1) {
            ok = false;
            if (printProblems) {
                std::cout << "Column id=" << (minEdgeId + col)
                          << " has -1:" << cnt_minus1 << ", +1:" << cnt_plus1 << "\n";
            }
        }
    }
    return ok;
}
