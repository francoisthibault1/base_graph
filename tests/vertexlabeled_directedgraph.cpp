#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"
#include "pgl/vertexlabeled_directedgraph.hpp"


using namespace std;

TEST(findVertexIndex, when_vertexAdded_expect_returnsProperIndex){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(1);
    graph.addVertex(3);
    graph.addVertex(2);
    
    EXPECT_EQ(graph.findVertexIndex(1), 0);
    EXPECT_EQ(graph.findVertexIndex(3), 1);
    EXPECT_EQ(graph.findVertexIndex(2), 2);
}

TEST(findVertexIndex, when_vertexDoesntExist_expect_throwLogicError){
    PGL::VertexLabeledDirectedGraph<int> graph;
    EXPECT_THROW(graph.findVertexIndex(0), logic_error);
    
    graph.addVertex(2);
    EXPECT_THROW(graph.findVertexIndex(1), logic_error);
}

TEST(isVertex, when_addingVertex_expect_returnsTrue){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(1);
    graph.addVertex(3);
    graph.addVertex(2);

    EXPECT_TRUE(graph.isVertex(1));
    EXPECT_TRUE(graph.isVertex(3));
    EXPECT_TRUE(graph.isVertex(2));
}

TEST(getOutEdges, when_vertexHasInAndOutEdges_expect_returnListWithOutEdges){
    PGL::VertexLabeledDirectedGraph<string> graph;
    graph.addVertex("A");
    graph.addVertex("B");
    graph.addVertex("C");
    graph.addEdge("A", "B");
    graph.addEdge("C", "A");

    EXPECT_EQ(graph.getOutEdgesOf("A"), list<string> ({"B"}));
}

TEST(isVertex, when_vertexDoesntExist_expect_returnFalse){
    PGL::VertexLabeledDirectedGraph<int> graph;
    EXPECT_FALSE(graph.isVertex(0));

    graph.addVertex(1);
    EXPECT_FALSE(graph.isVertex(0));
}

TEST(changeVertexIdentifierTo, when_changeVertexName_expect_newNameExistsAndOldNameDoesnt){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(0);
    graph.addVertex(1);
    graph.changeVertexObjectTo(0, 3);
    EXPECT_FALSE(graph.isVertex(0));
    EXPECT_TRUE(graph.isVertex(3));
}

TEST(removeVertexFromEdgeList, when_removeVertex_expect_edgesWithVertexDontExist){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(0);
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(0, 1);
    graph.addEdge(2, 1);
    graph.addEdge(3, 0);

    graph.removeVertexFromEdgeList(1);
    EXPECT_FALSE(graph.isEdge(2, 1));
    EXPECT_FALSE(graph.isEdge(0, 1));
    EXPECT_TRUE(graph.isEdge(3, 0));
}

TEST(removeEdge, when_removingEdge_expect_edgeDoesntExist){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(0);
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(0, 1);
    graph.addEdge(2, 1);
    graph.addEdge(1, 2);

    graph.removeEdge(1, 2);

    EXPECT_TRUE(graph.isEdge(0, 1));
    EXPECT_TRUE(graph.isEdge(2, 1));
    EXPECT_FALSE(graph.isEdge(1, 2));
}

TEST(removeVertexFromEdgeList, when_removingInexistentVertex_expect_throwLogicError){
    PGL::VertexLabeledDirectedGraph<int> graph;
    EXPECT_THROW(graph.removeVertexFromEdgeList(0), logic_error);
    graph.addVertex(1);
    EXPECT_THROW(graph.removeVertexFromEdgeList(0), logic_error);
}

TEST(removeMultiedges, when_removingMultiedge_expect_multiplicityOf1){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(0);
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 0);
    graph.addEdge(1, 2);
    graph.addEdge(1, 2, true);
    graph.addEdge(1, 2, true);
    graph.addEdge(1, 3);
    graph.addEdge(1, 0, true);
    graph.removeMultiedges();

    EXPECT_EQ(graph.getOutEdgesOf(1), list<int>({0, 2, 3}));
}


TEST(isEdge, when_addingEdge_expect_returnsTrueInOneDirection){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2);

    EXPECT_TRUE(graph.isEdge(1, 2));
    EXPECT_FALSE(graph.isEdge(2, 1));
}

TEST(isEdge, when_edgeDoesntExist_expect_returnsFalse){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(1);
    graph.addVertex(2);
    EXPECT_FALSE(graph.isEdge(1, 2));
    EXPECT_FALSE(graph.isEdge(2, 1));
}

TEST(ComparisonOperator, when_comparingTwoEmptyGraphs_expect_true){
    PGL::VertexLabeledDirectedGraph<int> graph;
    PGL::VertexLabeledDirectedGraph<int> graph2;
    EXPECT_TRUE(graph == graph2);
    EXPECT_TRUE(graph2 == graph);
}

TEST(ComparisonOperator, when_comparingDifferentNumberOfVerticesGraphs_expect_false){
    PGL::VertexLabeledDirectedGraph<int> graph;
    PGL::VertexLabeledDirectedGraph<int> graph2;
    graph.addVertex(1);
    graph.addVertex(2);
    graph2.addVertex(1);
    graph2.addVertex(2);
    graph2.addVertex(3);
    EXPECT_FALSE(graph == graph2);
    EXPECT_FALSE(graph2 == graph);
}

TEST(ComparisonOperator, when_comparingDifferentVertexOrderOfSameGraph_expect_true){
    PGL::VertexLabeledDirectedGraph<int> graph;
    PGL::VertexLabeledDirectedGraph<int> graph2;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 3);
    graph.addEdge(1, 2);

    graph2.addVertex(3);
    graph2.addVertex(1);
    graph2.addVertex(2);
    graph2.addEdge(1, 3);
    graph2.addEdge(1, 2);

    EXPECT_TRUE(graph == graph2);
    EXPECT_TRUE(graph2 == graph);
}

TEST(ComparisonOperator, when_comparingDifferentEdgeOrderOfSameGraph_expect_true){
    PGL::VertexLabeledDirectedGraph<int> graph;
    PGL::VertexLabeledDirectedGraph<int> graph2;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 3);
    graph.addEdge(1, 2);

    graph2.addVertex(3);
    graph2.addVertex(1);
    graph2.addVertex(2);
    graph2.addEdge(1, 2);
    graph2.addEdge(1, 3);

    EXPECT_TRUE(graph == graph2);
    EXPECT_TRUE(graph2 == graph);
}

TEST(ComparisonOperator, when_comparingGraphsWithDifferentVertices_expect_false){
    PGL::VertexLabeledDirectedGraph<int> graph;
    PGL::VertexLabeledDirectedGraph<int> graph2;
    graph.addVertex(1);
    graph.addVertex(4);
    graph.addVertex(3);
    graph.addEdge(1, 3);
    graph.addEdge(1, 4);

    graph2.addVertex(3);
    graph2.addVertex(1);
    graph2.addVertex(2);
    graph2.addEdge(1, 2);
    graph2.addEdge(1, 3);

    EXPECT_FALSE(graph == graph2);
    EXPECT_FALSE(graph2 == graph);
}

TEST(ComparisonOperator, when_comparingGraphsWithDifferentEdges_expect_false){
    PGL::VertexLabeledDirectedGraph<int> graph;
    PGL::VertexLabeledDirectedGraph<int> graph2;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);

    graph2.addVertex(1);
    graph2.addVertex(2);
    graph2.addVertex(3);
    graph2.addEdge(1, 3);
    EXPECT_FALSE(graph == graph2);
    EXPECT_FALSE(graph2 == graph);
}

TEST(ComparisonOperator, when_comparingGraphsWithOpposingDirectedEdges_expect_false){
    PGL::VertexLabeledDirectedGraph<int> graph;
    PGL::VertexLabeledDirectedGraph<int> graph2;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 2);
    graph.addEdge(3, 1);

    graph2.addVertex(1);
    graph2.addVertex(2);
    graph2.addVertex(3);
    graph.addEdge(1, 2);
    graph2.addEdge(1, 3);
    EXPECT_FALSE(graph == graph2);
    EXPECT_FALSE(graph2 == graph);
}

TEST(CopyConstructor, when_copyGraph_expect_ComparisonOperatorReturnTrue){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addVertex(4);

    graph.addEdge(1, 2);
    graph.addEdge(3, 1);

    PGL::VertexLabeledDirectedGraph<int> copiedNetwork(graph);
    EXPECT_TRUE(copiedNetwork == graph);
}

TEST(CopyConstructor, when_copyGraph_expect_validObjectAfterDestructionOfSource){
    /* Source graph declared in the heap because google test calls the destructor
     * at the end of the test. We want to destroy the object manually in the test case
     */
    PGL::VertexLabeledDirectedGraph<int>* graph = new PGL::VertexLabeledDirectedGraph<int>(); 
    graph->addVertex(1);
    graph->addVertex(2);
    graph->addVertex(3);
    graph->addVertex(4);
    graph->addEdge(1, 2);
    graph->addEdge(3, 1);

    PGL::VertexLabeledDirectedGraph<int> copiedNetwork(*graph);
    delete graph;

    EXPECT_TRUE(copiedNetwork.isVertex(1));
    EXPECT_TRUE(copiedNetwork.isVertex(2));
    EXPECT_TRUE(copiedNetwork.isVertex(3));
    EXPECT_TRUE(copiedNetwork.isVertex(4));

    EXPECT_TRUE(copiedNetwork.isEdge(1, 2));
    EXPECT_FALSE(copiedNetwork.isEdge(2, 1));
    EXPECT_TRUE(copiedNetwork.isEdge(3, 1));
    EXPECT_FALSE(copiedNetwork.isEdge(1, 3));
}

TEST(CopyConstructorFromBase, when_copyGraphFromBaseClass_expect_hasSameEdges){
    PGL::DirectedGraph graph(4);
    graph.addEdgeIdx(1, 2);
    graph.addEdgeIdx(3, 1);

    PGL::VertexLabeledDirectedGraph<int> templateCopy(graph, std::vector<int>({0, 1, 2, 3}));
    EXPECT_TRUE(templateCopy.isEdgeIdx(1, 2));
    EXPECT_FALSE(templateCopy.isEdgeIdx(2, 1));
    EXPECT_TRUE(templateCopy.isEdgeIdx(3, 1));
    EXPECT_FALSE(templateCopy.isEdgeIdx(1, 3));
}

TEST(EdgelistConstructor, when_constructingGraphFromEdgeList_expect_equalsManuallyCreatedGraph) {
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(10);
    graph.addVertex(8);
    graph.addEdge(1, 2);
    graph.addEdge(8, 1);
    graph.addEdge(1, 10);

    PGL::VertexLabeledDirectedGraph<int>
        graph2(list<pair<int, int>> ({ {1, 2}, {1, 10}, {8, 1} }));

    EXPECT_EQ(graph, graph2);
}

TEST(AssignementOperator, when_copyGraph_expect_ComparisonOperatorReturnTrue){
    PGL::VertexLabeledDirectedGraph<int> graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addVertex(4);

    graph.addEdge(1, 2);
    graph.addEdge(3, 1);

    PGL::VertexLabeledDirectedGraph<int> copiedNetwork = graph;
    EXPECT_TRUE(copiedNetwork == graph);
}

TEST(AssignementOperator, when_copyGraph_expect_validObjectAfterDestructionOfSource){
    /* Source graph declared in the heap because otherwise google test make a second call to the destructor
     * at the end of the test
     */
    PGL::VertexLabeledDirectedGraph<int>* graph = new PGL::VertexLabeledDirectedGraph<int>(); 
    graph->addVertex(1);
    graph->addVertex(2);
    graph->addVertex(3);
    graph->addVertex(4);
    graph->addEdge(1, 2);
    graph->addEdge(3, 1);

    PGL::VertexLabeledDirectedGraph<int> copiedNetwork;
    copiedNetwork = *graph;
    delete graph;

    EXPECT_TRUE(copiedNetwork.isVertex(1));
    EXPECT_TRUE(copiedNetwork.isVertex(2));
    EXPECT_TRUE(copiedNetwork.isVertex(3));
    EXPECT_TRUE(copiedNetwork.isVertex(4));

    EXPECT_TRUE(copiedNetwork.isEdge(1, 2));
    EXPECT_FALSE(copiedNetwork.isEdge(2, 1));
    EXPECT_TRUE(copiedNetwork.isEdge(3, 1));
    EXPECT_FALSE(copiedNetwork.isEdge(1, 3));
}