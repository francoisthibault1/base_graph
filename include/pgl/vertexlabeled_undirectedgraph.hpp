#ifndef PGL_VERTEX_LABELED_UNDIRECTED_GRAPH_HPP
#define PGL_VERTEX_LABELED_UNDIRECTED_GRAPH_HPP

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <utility>

#include "pgl/undirectedgraph.h"
#include "pgl/algorithms/graphpaths.h"


namespace PGL{

template<typename T>
class VertexLabeledUndirectedGraph: public UndirectedGraph{

    protected:
        std::vector<T> vertices;

    public:
        void debug() const;
        VertexLabeledUndirectedGraph() {size=0;};
        VertexLabeledUndirectedGraph(const std::list<std::pair<T, T>>& edgeList);
        VertexLabeledUndirectedGraph(const VertexLabeledUndirectedGraph<T>& source);
        VertexLabeledUndirectedGraph(const UndirectedGraph& source, const std::vector<T>& vertices);

        VertexLabeledUndirectedGraph<T> operator =(const VertexLabeledUndirectedGraph<T>& other);
        bool operator ==(const VertexLabeledUndirectedGraph<T>& other) const;
        bool operator!=(const VertexLabeledUndirectedGraph<T>& other) const { return !(this->operator==(other)); };


        void addVertex(T vertex, bool force=false);
        bool isVertex(T vertex) const;
        void changeVertexObjectTo(T currentObject, T newObject);
        void removeVertexFromEdgeList(T vertex) { removeVertexFromEdgeListIdx(findVertexIndex(vertex)); };
        T getVertexFromIdx(size_t vertexIdx);
        size_t findVertexIndex(T vertex) const;

        const size_t getSize() const {return size;}
        const std::vector<T>& getVertices() const { return vertices; };
        const std::list<T> getNeighboursOf(T vertex) const;

        std::list<T> convertIndicesListToObjects(std::list<size_t> indicesList) const;
        std::vector<T> convertIndicesVectorToObjects(std::vector<size_t> indicesVector) const;

        void addEdge(T source, T destination, bool force=false) { addEdgeIdx(findVertexIndex(source), findVertexIndex(destination)); }
        void removeEdge(T source, T destination) { removeEdgeIdx(findVertexIndex(source), findVertexIndex(destination)); };
        bool isEdge(T source, T destination) const { return isEdgeIdx(findVertexIndex(source), findVertexIndex(destination)); };

        size_t getDegree(T vertex) const { return getDegreeIdx(findVertexIndex(vertex)); };


        void writeEdgeListInTextFile(const std::string& fileName) const;
        void writeEdgeListInTextFile(std::ofstream& fileStream) const;
        void writeEdgeListInBinaryFile(const std::string& fileName, size_t byteSize=0) const;
        void writeEdgeListInBinaryFile(std::ofstream& fileStream, size_t byteSize=0) const;
        void writeVerticesInBinaryFile(const std::string& fileName, size_t byteSize=0) const;
        void writeVerticesInBinaryFile(std::ofstream& fileStream, size_t byteSize=0) const;

        static VertexLabeledUndirectedGraph<std::string> loadEdgeListFromTextFile(const std::string& fileName);
        static VertexLabeledUndirectedGraph<std::string> loadEdgeListFromTextFile(std::ifstream& fileStream);
        static VertexLabeledUndirectedGraph<T> loadEdgeListFromBinaryFile(const std::string& fileName, size_t byteSize=0);
        static VertexLabeledUndirectedGraph<T> loadEdgeListFromBinaryFile(std::ifstream& fileStream, size_t byteSize=0);
        void addVerticesFromBinaryFile(const std::string& fileName, size_t byteSize=0);
        void addVerticesFromBinaryFile(std::ifstream& fileStream, size_t byteSize=0);

        friend std::ostream& operator <<(std::ostream &stream, const VertexLabeledUndirectedGraph<T>& graph) {
                for (int i=0; i<graph.size; ++i){
                    stream << "Vertex " << graph.vertices[i] << ": ";
                    for (auto* neighbour: graph.getNeighboursOf(i))
                        stream << graph.vertices[neighbour] << ", ";
                    stream << "\n";
                }
                stream << "\n";
                return stream;
            };
};

template<typename T>
VertexLabeledUndirectedGraph<T>::VertexLabeledUndirectedGraph(const VertexLabeledUndirectedGraph<T>& source){
    vertices = std::vector<T>(source.vertices);
    vertices.resize(source.size);
    adjacencyList.resize(source.size);
    size = source.size;

    std::list<size_t>::const_iterator neighbour;
    for (size_t i=0; i<source.size; ++i){
        adjacencyList[i].clear();
        for (neighbour=source.adjacencyList[i].begin(); neighbour!=source.adjacencyList[i].end(); ++neighbour){
            addEdgeIdx(i, *neighbour);
        }
    }
}

template<typename T>
VertexLabeledUndirectedGraph<T>::VertexLabeledUndirectedGraph(const UndirectedGraph& source, const std::vector<T>& verticesNames){
    vertices = std::vector<T>(verticesNames);
    vertices.resize(vertices.size());
    adjacencyList.resize(vertices.size());
    size = vertices.size();

    std::list<size_t>::const_iterator neighbour;
    std::list<size_t> neighbourhood;
    for (size_t i=0; i<size; ++i){
        adjacencyList[i].clear();
        neighbourhood = source.getNeighboursOfIdx(i);
        for (neighbour=neighbourhood.begin(); neighbour!=neighbourhood.end(); ++neighbour){
            addEdgeIdx(i, *neighbour);
        }
    }
}


template<typename T>
VertexLabeledUndirectedGraph<T> VertexLabeledUndirectedGraph<T>::operator=(const VertexLabeledUndirectedGraph<T>& other){
    if (this != &other){
        vertices = std::vector<T>(other.vertices);
        size = other.size;

        std::list<size_t>::const_iterator neighbour;
        for (size_t i=0; i<other.size; ++i){
            adjacencyList[i].clear();
            for (neighbour=other.adjacencyList[i].begin(); neighbour!=other.adjacencyList[i].end(); ++neighbour){
                addEdgeIdx(i, *neighbour);
                addEdgeIdx(*neighbour, i);
            }
        }
    }
    return *this;
}

template<typename T>
bool VertexLabeledUndirectedGraph<T>::operator==(const VertexLabeledUndirectedGraph<T>& other) const{
    bool sameObject = size == other.size;
    
    std::list<size_t>::const_iterator it;
    try {
    for (size_t i=0; i<size && sameObject; ++i){
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
    } catch (std::logic_error) {
        sameObject = false;
    }

    return sameObject;
}

template <typename T>
void VertexLabeledUndirectedGraph<T>::addVertex(T vertex, bool force){
    if (force || !isVertex(vertex)){
        vertices.push_back(vertex);
        adjacencyList.push_back(std::list<size_t>());
        size++;
    }
}

template <typename T>
bool VertexLabeledUndirectedGraph<T>::isVertex(T vertex) const{
    bool exists = false;
    for (size_t i=0; i<size && !exists; ++i)
        if (vertices[i] == vertex) exists = true;
    return exists;
}

template<typename T>
const std::list<T> VertexLabeledUndirectedGraph<T>::getNeighboursOf(T vertex) const{
    return convertIndicesListToObjects(getNeighboursOfIdx(findVertexIndex(vertex)));
}

template<typename T>
std::list<T> VertexLabeledUndirectedGraph<T>::convertIndicesListToObjects(std::list<size_t> indicesList) const{
    std::list<T> returnedList;
    std::list<size_t>::iterator i;
    for (i=indicesList.begin(); i!=indicesList.end(); ++i){
        if (*i >= size) throw std::out_of_range("The given list is invalid: index greater than the vertices size.");
        returnedList.push_back(vertices[*i]);
    }
    return returnedList;
}

template<typename T>
std::vector<T> VertexLabeledUndirectedGraph<T>::convertIndicesVectorToObjects(std::vector<size_t> indicesVector) const{
    std::vector<T> returnedVector;
    returnedVector.resize(indicesVector.size());
    for (size_t i=0; i<indicesVector.size(); ++i){
        if (i >= size) throw std::out_of_range("The given list is invalid: index greater than the vertices size.");
        returnedVector[i] = vertices[i];
    }
    return returnedVector;
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::changeVertexObjectTo(T currentObject, T newObject){
    if (isVertex(newObject)) throw std::logic_error("The object is already used as an attribute by another vertex.");
    vertices[findVertexIndex(currentObject)] = newObject;
}

template<typename T>
size_t VertexLabeledUndirectedGraph<T>::findVertexIndex(T vertex) const{
    for (size_t i=0; i<size; ++i){
        if (vertices[i] == vertex)
            return i;
    }
    throw std::logic_error("Vertex does not exist");
}

template<typename T>
T VertexLabeledUndirectedGraph<T>::getVertexFromIdx(size_t vertexIdx) {
    if (vertexIdx >= size) throw std::out_of_range("The given list is invalid: index greater than the vertices size.");
    return vertices[vertexIdx];
}

// File i/o
template<typename T>
VertexLabeledUndirectedGraph<std::string> VertexLabeledUndirectedGraph<T>::loadEdgeListFromTextFile(const std::string& fileName){
    VertexLabeledUndirectedGraph<std::string> returnedGraph;

    std::ifstream fileStream(fileName.c_str());
    returnedGraph = loadEdgeListFromTextFile(fileStream);
    fileStream.close();

    return returnedGraph;
}

template<typename T>
VertexLabeledUndirectedGraph<std::string> VertexLabeledUndirectedGraph<T>::loadEdgeListFromTextFile(std::ifstream& fileStream){
    VertexLabeledUndirectedGraph<std::string> returnedGraph;

    std::stringstream currentLine;
    std::string full_line, name1_str, name2_str;

    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");
    else {
        while( std::getline(fileStream, full_line)){
            fileStream >> std::ws;
            currentLine.str(full_line);
            currentLine >> std::ws;
            currentLine >> name1_str >> std::ws;

            // Skips a line of comment.
            if(name1_str == "#") {
                currentLine.clear();
                continue;
            }
            currentLine >> name2_str >> std::ws;
            currentLine.clear();

            returnedGraph.addVertex(name1_str);
            returnedGraph.addVertex(name2_str);
            returnedGraph.addEdge(name1_str, name2_str);
        }
    }
    return returnedGraph;
}

template<typename T>
VertexLabeledUndirectedGraph<T> VertexLabeledUndirectedGraph<T>::loadEdgeListFromBinaryFile(const std::string& fileName, size_t byteSize){
    VertexLabeledUndirectedGraph<T> returnedGraph;

    std::ifstream fileStream(fileName.c_str(), std::ios::in | std::ios::binary);
    returnedGraph = loadEdgeListFromBinaryFile(fileStream);
    fileStream.close();

    return returnedGraph;
}

template<typename T>
VertexLabeledUndirectedGraph<T> VertexLabeledUndirectedGraph<T>::loadEdgeListFromBinaryFile(std::ifstream& fileStream, size_t byteSize){
    VertexLabeledUndirectedGraph<T> returnedGraph;
    if (byteSize == 0) byteSize = sizeof(T);

    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");

    T vertex1, vertex2;
    size_t i = 0;
    while (fileStream.read((char*) &vertex2, byteSize)){
        returnedGraph.addVertex(vertex2);
        if (i % 2 == 1)
            returnedGraph.addEdge(vertex1, vertex2);
        vertex1 = vertex2;
        ++i;
    }
    return returnedGraph;
}

template<>
VertexLabeledUndirectedGraph<std::string> VertexLabeledUndirectedGraph<std::string>::loadEdgeListFromBinaryFile(std::ifstream& fileName, size_t byteSize){
    throw std::logic_error("No implementation of string binary files.");
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::addVerticesFromBinaryFile(const std::string& fileName, size_t byteSize){
    std::ifstream fileStream(fileName.c_str(), std::ios::binary);
    addVerticesFromBinaryFile(fileStream, byteSize);
    fileStream.close();
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::addVerticesFromBinaryFile(std::ifstream& fileStream, size_t byteSize){
    if (byteSize == 0) byteSize = sizeof(T);

    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");

    T vertex;
    while (fileStream.read((char*) &vertex, byteSize)){
        addVertex(vertex);
    }
}

template<>
void VertexLabeledUndirectedGraph<std::string>::addVerticesFromBinaryFile(std::ifstream& fileStream, size_t byteSize){
    throw std::logic_error("No implementation of string binary files.");
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::writeEdgeListInTextFile(const std::string& fileName) const{
    std::ofstream fileStream(fileName.c_str());
    writeEdgeListInTextFile(fileStream);
    fileStream.close();
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::writeEdgeListInTextFile(std::ofstream& fileStream) const{
    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");

    fileStream << "# Vertex1,  Vertex2\n";
    std::list<size_t>::const_iterator j;
    for (size_t i=0; i<size; ++i){
        for (j=adjacencyList[i].begin(); j!=adjacencyList[i].end(); ++j){
            // Cast to int because operator << does not output properly otherwise
            if (i<*j) fileStream << vertices[i] << "   " << vertices[*j] << '\n';
        }
    }
}

template<>
void VertexLabeledUndirectedGraph<unsigned char>::writeEdgeListInTextFile(std::ofstream& fileStream) const{
    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");

    fileStream << "# Vertex1,  Vertex2\n";
    std::list<size_t>::const_iterator j;
    for (size_t i=0; i<size; ++i){
        for (j=adjacencyList[i].begin(); j!=adjacencyList[i].end(); ++j){
            // Cast to int because operator << does not output properly otherwise
            if (i<*j) fileStream << (unsigned long int) vertices[i] << "   " << (unsigned long int) vertices[*j] << '\n';
        }
    }
}

template<>
void VertexLabeledUndirectedGraph<signed char>::writeEdgeListInTextFile(std::ofstream& fileStream) const{
    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");

    fileStream << "# Vertex1,  Vertex2\n";
    std::list<size_t>::const_iterator j;
    for (size_t i=0; i<size; ++i){
        for (j=adjacencyList[i].begin(); j!=adjacencyList[i].end(); ++j){
            if (i<*j) fileStream << (unsigned long int) vertices[i] << "   " << (unsigned long int) vertices[*j] << '\n';
        }
    }
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::writeEdgeListInBinaryFile(const std::string& fileName, size_t byteSize) const{
    std::ofstream fileStream(fileName.c_str(), std::ios::out | std::ios::binary);
    writeEdgeListInBinaryFile(fileStream, byteSize);
    fileStream.close();
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::writeEdgeListInBinaryFile(std::ofstream& fileStream, size_t byteSize) const{
    if (byteSize == 0) byteSize = sizeof(T);

    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");

    std::list<size_t>::const_iterator j;
    for (size_t i=0; i<size; ++i){
        for (j=adjacencyList[i].begin(); j != adjacencyList[i].end(); ++j){
            if (i <= *j) { // write edges once
                fileStream.write((char*) &vertices[i], byteSize);
                fileStream.write((char*) &vertices[*j], byteSize);
            }
        }
    }
}

template<>
void VertexLabeledUndirectedGraph<std::string>::writeEdgeListInBinaryFile(std::ofstream& fileName, size_t byteSize) const{
    throw std::logic_error("No implementation of string binary files.");
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::writeVerticesInBinaryFile(const std::string& fileName, size_t byteSize) const{
    std::ofstream fileStream(fileName, std::ios::binary);
    writeVerticesInBinaryFile(fileStream, byteSize);
    fileStream.close();
}

template<typename T>
void VertexLabeledUndirectedGraph<T>::writeVerticesInBinaryFile(std::ofstream& fileStream, size_t byteSize) const{
    if (byteSize == 0) byteSize = sizeof(T);

    if(!fileStream.is_open())
        throw std::runtime_error("Could not open file.");

    for (size_t i=0; i<size; ++i){
        fileStream.write((char*) &vertices[i], byteSize);
    }
}

template<>
void VertexLabeledUndirectedGraph<std::string>::writeVerticesInBinaryFile(std::ofstream& fileStream, size_t byteSize) const{
    throw std::logic_error("No implementation of string binary files.");
}

} // namespace PGL

#endif
