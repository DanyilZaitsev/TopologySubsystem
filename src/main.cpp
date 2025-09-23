#include "Graph/Graph.h"
#include "GraphLoad/GraphLoad.cpp"
#include "Graph/Decomposition.cpp"
#include "Graph/IncidenceMatrix.cpp"
#include "Graph/ContourMatrix.cpp"

#include <unordered_map>
#include <iostream>

// ../resources/Graph.csv
int main() {

    std::string filename = "../resources/Graph.csv";
    //std::cout << "Enter the name of the file with the graph: ";
    //std::cin >> filename;

    try {
        auto edges1 = GraphLoader::loadAuto(filename);

        Graph graph(edges1);
        Decomposition dec(graph);
        IncidenceMatrix mat(graph);
        ContourMatrix CM(graph, dec);

        dec.build();
        //dec.print();

        std::cout << "-----------------------------------------------------------------------------" << "\n";

        //mat.print();

        if (!mat.validate()) {
            std::cout << "Incidence matrix validation failed: some columns don't have exactly one -1 and one +1.\n";
        }

        CM.build();
        //CM.validate();
        CM.printSparse();
        //CM.printDense();



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
