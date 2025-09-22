#ifndef INCIDENCE_MATRIX_H
#define INCIDENCE_MATRIX_H

#include <vector>
#include <iostream>
#include "Graph.h"

class IncidenceMatrix {
private:
    std::vector<std::vector<int>> matrix;

public:
    IncidenceMatrix(const Graph& graph);

    const std::vector<std::vector<int>>& getMatrix() const;

    void print() const;

    void validateAndPrintSummary() const;
};

#endif
