#ifndef BASE_GRAPH_VERTEX_LABELED_UNDIRECTED_GRAPH_HPP
#define BASE_GRAPH_VERTEX_LABELED_UNDIRECTED_GRAPH_HPP

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>

#include <vector>

#include "BaseGraph/undirectedgraph.h"
#include "BaseGraph/algorithms/graphpaths.h"


namespace BaseGraph{

template<typename T>
class VertexLabeledUndirectedGraph: public UndirectedGraph{

    protected:
        std::vector<T> vertices;

    public:
        VertexLabeledUndirectedGraph(): UndirectedGraph(0) {};
        VertexLabeledUndirectedGraph(const std::list<std::pair<T, T>>& edgeList);
        VertexLabeledUndirectedGraph(const UndirectedGraph& source, const std::vector<T>& vertices);

        bool operator==(const VertexLabeledUndirectedGraph<T>& other) const;
        bool operator!=(const VertexLabeledUndirectedGraph<T>& other) const { return !(this->operator==(other)); };


        void addVertex(T vertex, bool force=false);
        bool isVertex(T vertex) const;
        void changeVertexObjectTo(T currentObject, T newObject);
        void removeVertexFromEdgeList(T vertex) { removeVertexFromEdgeListIdx(findVertexIndex(vertex)); };
        const T& getVertexFromIdx(VertexIndex vertexIdx) const;
        VertexIndex findVertexIndex(T vertex) const;

        const std::vector<T>& getVertices() const { return vertices; };
        std::list<T> getNeighboursOf(T vertex) const;

        std::list<T> convertIndicesListToObjects(const std::list<VertexIndex>& indicesList) const;
        std::vector<T> convertIndicesVectorToObjects(const std::vector<VertexIndex>& indicesVector) const;

        void addEdge(T source, T destination, bool force=false) { addEdgeIdx(findVertexIndex(source), findVertexIndex(destination)); }
        void removeEdge(T source, T destination) { removeEdgeIdx(findVertexIndex(source), findVertexIndex(destination)); };
        bool isEdge(T source, T destination) const { return isEdgeIdx(findVertexIndex(source), findVertexIndex(destination)); };

        size_t getDegree(T vertex) const { return getDegreeIdx(findVertexIndex(vertex)); };


        friend std::ostream& operator <<(std::ostream &stream, const VertexLabeledUndirectedGraph<T>& graph) {
            stream << "Vertex labeled undirected graph of size: " << graph.getSize() << "\n"
                   << "Neighbours of:\n";

            for (VertexIndex i: graph) {
                stream << graph.vertices[i] << ": ";
                for (auto& neighbour: graph.getOutEdgesOfIdx(i))
                    stream << graph.vertices[neighbour] << ", ";
                stream << "\n";
            }
            return stream;
        }
};

template<typename T>
VertexLabeledUndirectedGraph<T>::VertexLabeledUndirectedGraph(const std::list<std::pair<T, T>>& edgeList):
        UndirectedGraph(0) {
    for (auto& edge: edgeList) {
        // By default addVertex does not add existing labels
        addVertex(edge.first);
        addVertex(edge.second);
        addEdge(edge.first, edge.second);
    }
}

template<typename T>
VertexLabeledUndirectedGraph<T>::VertexLabeledUndirectedGraph(const UndirectedGraph& source, const std::vector<T>& verticesNames):
        UndirectedGraph(source.getSize()) {
    if (source.getSize() != verticesNames.size())
        throw std::logic_error("The vertices vector must be the size of the graph");

    vertices = verticesNames;
    edgeNumber = 0;

    for (VertexIndex& vertex: source)
        for (const VertexIndex& neighbour: source.getNeighboursOfIdx(vertex))
            addEdgeIdx(vertex, neighbour);
}

template<typename T>
bool VertexLabeledUndirectedGraph<T>::operator==(const VertexLabeledUndirectedGraph<T>& other) const{
    bool sameObject = size == other.size;

    std::list<VertexIndex>::const_iterator it;
    try {
    for (VertexIndex i=0; i<size && sameObject; ++i){
        if (!other.isVertex(vertices[i])) sameObject = false;
        if (!isVertex(other.vertices[i])) sameObject = false;

        for (it=adjacencyList[i].begin(); it != adjacencyList[i].end() && sameObject; ++it){
            if (!other.isEdge(vertices[i], vertices[*it]))
                sameObject = false;
        }

        for (it=other.adjacencyList[i].begin(); it != other.adjacencyList[i].end() && sameObject; ++it){
            if (!isEdge(other.vertices[i], other.vertices[*it]))
                sameObject = false;
        }
    }
    } catch (std::logic_error&) {  // isEdge calling findVertexIndex threw "Vertex does not exist"
        sameObject = false;
    }

    return sameObject;
}

template <typename T>
void VertexLabeledUndirectedGraph<T>::addVertex(T vertex, bool force){
    if (force || !isVertex(vertex)) {
        vertices.push_back(vertex);
        adjacencyList.push_back(std::list<VertexIndex>());
        size++;
    }
}

template <typename T>
bool VertexLabeledUndirectedGraph<T>::isVertex(T vertex) const{
    bool exists = false;
    for (VertexIndex i=0; i<size && !exists; ++i)
        if (vertices[i] == vertex) exists = true;
    return exists;
}

template<typename T>
std::list<T> VertexLabeledUndirectedGraph<T>::getNeighboursOf(T vertex) const{
    return convertIndicesListToObjects(getNeighboursOfIdx(findVertexIndex(vertex)));
}

template<typename T>
std::list<T> VertexLabeledUndirectedGraph<T>::convertIndicesListToObjects(const std::list<VertexIndex>& indicesList) const{
    std::list<T> returnedList;

    for (auto& element: indicesList) {
        if (element >= size) throw std::out_of_range("The given list is invalid: index greater than the vertices size.");
        returnedList.push_back(vertices[element]);
    }
    return returnedList;
}

template<typename T>
std::vector<T> VertexLabeledUndirectedGraph<T>::convertIndicesVectorToObjects(const std::vector<VertexIndex>& indicesVector) const{
    std::vector<T> returnedVector(indicesVector.size());

    for (auto& element: indicesVector) {
        if (element >= size) throw std::out_of_range("The given list is invalid: index greater than the vertices size.");
        returnedVector[element] = vertices[element];
    }
    return returnedVector;
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::changeVertexObjectTo(T currentObject, T newObject){
    if (isVertex(newObject)) throw std::logic_error("The object is already used as an attribute by another vertex.");
    vertices[findVertexIndex(currentObject)] = newObject;
}

template<typename T>
VertexIndex VertexLabeledUndirectedGraph<T>::findVertexIndex(T vertex) const{
    for (VertexIndex& i: *this)
        if (vertices[i] == vertex)
            return i;
    throw std::logic_error("Vertex does not exist");
}

template<typename T>
const T& VertexLabeledUndirectedGraph<T>::getVertexFromIdx(VertexIndex vertexIdx) const {
    if (vertexIdx >= size) throw std::out_of_range("The given list is invalid: index greater than the vertices size.");
    return vertices[vertexIdx];
}


} // namespace BaseGraph

#endif