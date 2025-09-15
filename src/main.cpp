#include "Graph/Graph.cpp"
#include "GraphLoad/GraphLoad.cpp"

int main() {
    /*std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 2}, {2, 0}, {2, 3}, {3, 3}
    };

    Graph graph(edges);
    graph.printGraph();

    int v = 2;
    auto neighbors = graph.getNeighbors(v);
    std::cout << "Соседи вершины " << v << ": ";
    for (int n : neighbors) {
        std::cout << n << " ";
    }
    std::cout << "\n";*/

    std::string filename;
    std::cout << "Enter the name of the file with the graph: ";
    std::cin >> filename;

    try {
        auto edges = GraphLoader::loadAuto(filename);

        Graph graph(edges);
        graph.printGraph();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}