#include <queue>
#include <unordered_map>

#include "BaseGraph/undirectedgraph.h"
#include "BaseGraph/metrics/general.h"
#include "BaseGraph/algorithms/graphpaths.h"


using namespace std;


namespace BaseGraph{


template <typename T>
static double getClosenessCentralityOfVertexIdx(const T& graph, VertexIndex vertexIdx){
    vector<size_t> shortestPathLengths = findShortestPathLengthsFromVertexIdx(graph, vertexIdx);
    size_t componentSize = 0;
    unsigned long long int sum = 0;

    for (VertexIndex& vertex: graph) {
        if (shortestPathLengths[vertex] != SIZE_T_MAX){
            componentSize += 1;
            sum += shortestPathLengths[vertex];
        }
    }
    return sum > 0 ? ((double) componentSize-1)/sum : 0;
}

template <typename T>
vector<double> getClosenessCentralities(const T& graph) {
    vector<double> closenessCentralities(graph.getSize(), 0);

    for (VertexIndex vertex: graph)
        closenessCentralities[vertex] = getClosenessCentralityOfVertexIdx(graph, vertex);

    return closenessCentralities;
}

template <typename T>
static double getHarmonicCentralityOfVertexIdx(const T& graph, VertexIndex vertexIdx) {
    vector<size_t> shortestPathLengths = findShortestPathLengthsFromVertexIdx(graph, vertexIdx);

    double harmonicSum = 0;
    for (VertexIndex& vertex: graph)
        if (shortestPathLengths[vertex] != 0 && shortestPathLengths[vertex] != SIZE_T_MAX)
            harmonicSum += 1.0/shortestPathLengths[vertex];

    return harmonicSum;
}

template <typename T>
vector<double> getHarmonicCentralities(const T& graph) {
    vector<double> harmonicCentralities(graph.getSize(), 0);

    for (VertexIndex vertex: graph)
        harmonicCentralities[vertex] = getHarmonicCentralityOfVertexIdx(graph, vertex);

    return harmonicCentralities;
}

template <>
vector<double> getBetweennessCentralities(const DirectedGraph& graph, bool normalizeWithGeodesicNumber) {
    size_t verticesNumber = graph.getSize();
    vector<double> betweennesses;
    betweennesses.resize(verticesNumber, 0);

    MultiplePredecessors distancesPredecessors;
    list<list<VertexIndex> > currentGeodesics;
    for (const VertexIndex& i: graph) {
        distancesPredecessors = findAllPredecessorsOfVertexIdx(graph, i);
        for (const VertexIndex& j: graph) {
            currentGeodesics = findMultiplePathsToVertexFromPredecessorsIdx(graph, i, j, distancesPredecessors);
            if (currentGeodesics.empty()) continue; // vertices i and j are not in the same component

            for (auto& geodesic: currentGeodesics) {
                for (auto& vertexOnGeodesic: geodesic) {
                    if (vertexOnGeodesic != i && vertexOnGeodesic != j) {
                        if (normalizeWithGeodesicNumber)
                            betweennesses[vertexOnGeodesic] += 1./currentGeodesics.size();
                        else
                            betweennesses[vertexOnGeodesic] += 1;
                    }
                }
            }
        }
    }
    return betweennesses;
}

template <>
vector<double> getBetweennessCentralities(const UndirectedGraph& graph, bool normalizeWithGeodesicNumber) {
    size_t verticesNumber = graph.getSize();
    vector<double> betweennesses;
    betweennesses.resize(verticesNumber, 0);

    MultiplePredecessors distancesPredecessors;
    list<list<VertexIndex> > currentGeodesics;
    for (const VertexIndex& i: graph) {
        distancesPredecessors = findAllPredecessorsOfVertexIdx(graph, i);
        for (const VertexIndex& j: graph) {
            if (i>=j) continue;

            currentGeodesics = findMultiplePathsToVertexFromPredecessorsIdx(graph, i, j, distancesPredecessors);
            if (currentGeodesics.empty()) continue; // vertices i and j are not in the same component

            for (auto& geodesic: currentGeodesics) {
                for (auto& vertexOnGeodesic: geodesic) {
                    if (vertexOnGeodesic != i && vertexOnGeodesic != j) {
                        if (normalizeWithGeodesicNumber)
                            betweennesses[vertexOnGeodesic] += 1./currentGeodesics.size();
                        else
                            betweennesses[vertexOnGeodesic] += 1;
                    }
                }
            }
        }
    }
    return betweennesses;
}

template <typename T>
vector<size_t> getDiameters(const T& graph){
    size_t verticesNumber = graph.getSize();
    vector<size_t> diameters;
    diameters.resize(verticesNumber);

    vector<size_t> shortestPathLengths;
    size_t largestDistance;
    for (const VertexIndex& i: graph) {
        shortestPathLengths = findShortestPathLengthsFromVertexIdx(graph, i);
        largestDistance = shortestPathLengths[0];

        for (const VertexIndex& j: graph)
           if (shortestPathLengths[j] > largestDistance && shortestPathLengths[j]!=SIZE_T_MAX)
              largestDistance = shortestPathLengths[j];

        if (largestDistance == SIZE_T_MAX)
            diameters[i] = 0;
        else
            diameters[i] = largestDistance;
    }
    return diameters;
}

template <typename T>
static double getShortestPathAverageOfVertexIdx(const T& graph, VertexIndex vertexIdx) {
    vector<size_t> shortestPathLengths = findShortestPathLengthsFromVertexIdx(graph, vertexIdx);

    size_t sum = 0;
    size_t componentSize = 1;

    for (VertexIndex& vertex: graph)
        if (shortestPathLengths[vertex] != 0 && shortestPathLengths[vertex] != SIZE_T_MAX) {
            sum += shortestPathLengths[vertex];
            componentSize++;
        }

    return componentSize > 1 ? (double) sum / componentSize : 0;
}

template <typename T>
vector<double> getShortestPathAverages(const T& graph) {
    vector<double> averageShortestPaths(graph.getSize(), 0);

    for (VertexIndex vertex: graph)
        averageShortestPaths[vertex] = getShortestPathAverageOfVertexIdx(graph, vertex);

    return averageShortestPaths;
}

template <typename T>
vector<unordered_map<size_t, double> > getShortestPathsDistribution(const T& graph) {
    auto connectedComponents = findConnectedComponents(graph);

    vector<size_t> shortestPathLengths;
    vector<unordered_map<size_t, double> > shortestPathDistribution(connectedComponents.size());
    size_t componentIndex = 0;

    for (auto component: connectedComponents) {
        auto& currentDistribution = shortestPathDistribution[componentIndex];

        if (component.size() > 1) {
            for (const VertexIndex& vertex: component) {
                shortestPathLengths = findShortestPathLengthsFromVertexIdx(graph, vertex);

                for (const size_t& pathLength: shortestPathLengths) {
                    if (pathLength!=0 && pathLength!=SIZE_T_MAX) {
                        if (currentDistribution.find(pathLength) == currentDistribution.end())
                            currentDistribution[pathLength] = 1;
                        else
                            currentDistribution[pathLength]++;
                    }
                }
            }
            for (auto& element: currentDistribution)
                element.second /= component.size();
        }

        componentIndex++;
    }
    return shortestPathDistribution;
}

template <typename T>
static double getShortestPathHarmonicAverageOfVertexIdx(const T& graph, VertexIndex vertexIdx){
    vector<size_t> shortestPathLengths = findShortestPathLengthsFromVertexIdx(graph, vertexIdx);
    size_t componentSize = 0;

    double sumOfInverse = 0;
    for (VertexIndex& vertex: graph) {
        if (shortestPathLengths[vertex] != 0 && shortestPathLengths[vertex] != SIZE_T_MAX){
            componentSize += 1;
            sumOfInverse += 1.0/shortestPathLengths[vertex];
        }
    }
    return componentSize > 0 ? sumOfInverse/componentSize : 0;
}

template <typename T>
vector<double> getShortestPathHarmonicAverages(const T& graph) {
    vector<double> harmonicAverages(graph.getSize(), 0);

    for (VertexIndex vertex: graph)
        harmonicAverages[vertex] = getShortestPathHarmonicAverageOfVertexIdx(graph, vertex);

    return harmonicAverages;
}

template <typename T>
list<Component> findConnectedComponents(const T& graph){
    size_t verticesNumber = graph.getSize();
    if (verticesNumber == 0) throw logic_error("There are no vertices.");

    list<Component> connectedComponents;
    Component currentComponent;
    VertexIndex currentVertex, startVertex;

    queue<VertexIndex> verticesToProcess;
    vector<bool> processedVertices;
    bool allVerticesProcessed = false;
    processedVertices.resize(verticesNumber, false);

    list<VertexIndex>::const_iterator vertexNeighbour;

    while (!allVerticesProcessed){
        allVerticesProcessed = true;
        for (VertexIndex i=0; i<verticesNumber && allVerticesProcessed; ++i){
            if (!processedVertices[i]) {
                allVerticesProcessed = false;
                startVertex = i;
            }
        }

        if (!allVerticesProcessed){
            currentComponent.clear();
            verticesToProcess.push(startVertex);
            processedVertices[startVertex] = true;

            while (!verticesToProcess.empty()) {
                currentVertex = verticesToProcess.front();

                for (const VertexIndex& vertexNeighbour: graph.getOutEdgesOfIdx(currentVertex)) {
                    if (!processedVertices[vertexNeighbour]) {
                        verticesToProcess.push(vertexNeighbour);
                        processedVertices[vertexNeighbour] = true;
                    }
                }
                currentComponent.push_back(currentVertex);
                verticesToProcess.pop();
            }
            connectedComponents.push_back(currentComponent);
        }
    }
    return connectedComponents;
}

// Allowed classes for metrics

template vector<double> getClosenessCentralities(const DirectedGraph& graph);
template vector<double> getClosenessCentralities(const UndirectedGraph& graph);
template vector<double> getHarmonicCentralities(const DirectedGraph& graph);
template vector<double> getHarmonicCentralities(const UndirectedGraph& graph);

template vector<size_t> getDiameters(const DirectedGraph& graph);
template vector<size_t> getDiameters(const UndirectedGraph& graph);
template vector<double> getShortestPathAverages(const DirectedGraph& graph);
template vector<double> getShortestPathAverages(const UndirectedGraph& graph);
template vector<double> getShortestPathHarmonicAverages(const DirectedGraph& graph);
template vector<double> getShortestPathHarmonicAverages(const UndirectedGraph& graph);
template vector<unordered_map<size_t, double>> getShortestPathsDistribution(const DirectedGraph& graph);
template vector<unordered_map<size_t, double>> getShortestPathsDistribution(const UndirectedGraph& graph);

template list<Component> findConnectedComponents(const DirectedGraph& graph);
template list<Component> findConnectedComponents(const UndirectedGraph& graph);


} // namespace BaseGraph
