#include "graph.h"

template <typename EdgeType> inline Vertex<EdgeType>::Vertex() : id(num_vertices++), visited(0) {}


template <typename VertexType> inline UndirectedEdge<VertexType>::UndirectedEdge(VertexType *first, VertexType *second)
    : std::pair<VertexType *, VertexType *>(std::make_pair(first, second)) {}


template <typename VertexType> DirectedEdge<VertexType>::DirectedEdge(VertexType *from, VertexType *to) : from(from), to(to) {}


template <typename VertexType, typename EdgeType> Graph<VertexType, EdgeType>::~Graph() {
    for (VertexType *vtx : vertices_) {
        delete vtx;
    }
    for (EdgeType *edge : edges_) {
        delete edge;
    }
}
template <typename VertexType, typename EdgeType> inline const std::vector<VertexType *> &Graph<VertexType, EdgeType>::vertices() const {
    return vertices_;
}
template <typename VertexType, typename EdgeType> inline const std::vector<EdgeType *> &Graph<VertexType, EdgeType>::edges() const {
    return edges_;
}