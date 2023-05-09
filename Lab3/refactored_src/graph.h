#pragma once
#include <vector>
#include <string>
#include <type_traits>


template <typename EdgeType>
struct Vertex {
    inline static size_t num_vertices = 0;
    Vertex();

    int id, visited;
    std::vector<EdgeType *> edges;
};

template <typename VertexType>
struct UndirectedEdge : std::pair<VertexType *, VertexType *> {
    UndirectedEdge(VertexType *first = nullptr, VertexType *second = nullptr);
};

template <typename VertexType>
struct DirectedEdge {
    DirectedEdge(VertexType *from = nullptr, VertexType *to = nullptr);

    VertexType *from, *to;
};

template <typename VertexType, typename EdgeType>
class Graph {
public:
    virtual ~Graph();
    virtual void AddVertex(VertexType *vtx) = 0;
    virtual void AddEdge(EdgeType *edge) = 0;

    const std::vector<VertexType *> &vertices() const;
    const std::vector<EdgeType *> &edges() const;

protected:
    std::vector<VertexType *> vertices_;
    std::vector<EdgeType *> edges_;
};

template <typename VertexType, typename EdgeType>
class UndirectedGraph : public Graph<VertexType, EdgeType> {
    static_assert(std::is_base_of_v<UndirectedEdge<VertexType>, EdgeType>, "VertexType and EdgeType mismatch or EdgeType is not an UndirectedEdge.");
};

template <typename VertexType, typename EdgeType>
class DirectedGraph : public Graph<VertexType, EdgeType> {
    static_assert(std::is_base_of_v<DirectedEdge<VertexType>, EdgeType>, "VertexType and EdgeType mismatch or EdgeType is not an UndirectedEdge.");
};

#include "graph.tcc"