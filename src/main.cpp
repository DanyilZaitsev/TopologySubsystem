#include "Graph/Graph.h"
#include "GraphLoad/GraphLoad.cpp"
#include "Graph/Decomposition.cpp"

#include <unordered_map>
#include <iostream>

// ../resources/Graph.csv
int main() {

    std::string filename = "../resources/Graph.csv";
    //std::cout << "Enter the name of the file with the graph: ";
    //std::cin >> filename;

    try {
        auto edges = GraphLoader::loadAuto(filename);

        Graph graph(edges);

        Decomposition dec(graph);
        dec.build();
        const auto& bm = dec.getBaum()[5];

        int u = std::get<0>(bm);
        int v = std::get<1>(bm);
        int id = std::get<2>(bm);

        std::cout << "Edge: " << u << " - " << v << " (id=" << id << ")\n";
        //dec.print();


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
