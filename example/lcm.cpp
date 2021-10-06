#include <iostream>
#include "BaseGraph/undirectedgraph.h"
#include "BaseGraph/metrics/undirected.h"
#include "BaseGraph/algorithms/layeredconfigurationmodel.h"
#include "BaseGraph/fileio.h"

using namespace BaseGraph;
using namespace std;

template<typename T>
void printVector(const vector<T>& v) {
    for (auto x: v)
        cout << x << ",";
    cout << endl;
}

int main() {
    // BaseGraph::UndirectedGraph graph(6);
    // graph.addEdgeIdx(0, 1);
    // graph.addEdgeIdx(0, 2);
    // graph.addEdgeIdx(1, 2);
    // graph.addEdgeIdx(1, 3);
    // graph.addEdgeIdx(2, 4);
    // graph.addEdgeIdx(3, 4);
    // graph.addEdgeIdx(4, 5);


    auto graph = loadUndirectedEdgeListFromTextFile("test_ER.txt");

    std::cout << graph << std::endl;

    auto kShellsAndOnionLayers = getKShellsAndOnionLayers(graph);
    auto graphOG = graph;
    // shuffleGraphWithLayeredConfigurationModel(graph, 100);

    for (size_t i=0; i<1000000; i++) {

        shuffleGraphWithLayeredConfigurationModel(graph, 1);
        // std::cout << i << std::endl;

        if (getOnionLayers(graph)!=kShellsAndOnionLayers.second) {

            std::cout << "Onion Decomposition not conserved." << std::endl;
            // cout << graph << endl;
            // cout << graphOG << endl;
            // printVector(kShellsAndOnionLayers.first);
            // printVector(getKShells(graph));
            // printVector(kShellsAndOnionLayers.second);
            // printVector(getOnionLayers(graph));
            break;

        }

    }

    // std::cout << graph << std::endl;

    
    return 0;
}
