#include "Graph/Graph.h"
#include "GraphLoad/GraphLoad.cpp"
#include "Graph/Decomposition.cpp"

#include <unordered_map>
#include <iostream>

int main() {

    std::string filename;
    std::cout << "Enter the name of the file with the graph: ";
    std::cin >> filename;

    try {
        auto edges = GraphLoader::loadAuto(filename);

        Graph graph(edges);

        Decomposition dec(graph);
        dec.build();
        dec.print();


        //graph.printGraph();
        //graph.getNeighbors(50);

        /*int branchId = 45;
        try {
            double rj = graph.getAttribute(branchId, "Rj");
            double fj = graph.getAttribute(branchId, "Fj");
            std::cout << "Branch " << branchId << ":\n";
            std::cout << "  Rj = " << rj << "\n";
            std::cout << "  Fj = " << fj << "\n";
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }*/

        
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
