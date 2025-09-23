#ifndef INCIDENCE_MATRIX_H
#define INCIDENCE_MATRIX_H

#include <vector>
#include <iostream>
#include "Graph.h"

class IncidenceMatrix {
private:
    std::vector<std::vector<int>> matrix;

    int minEdgeId = 0;
    int maxEdgeId = -1;

public:
    IncidenceMatrix(const Graph& graph);

    const std::vector<std::vector<int>>& getMatrix() const { return matrix; };

    void print() const;

    bool validate(bool printProblems = true) const;
};

#endif
