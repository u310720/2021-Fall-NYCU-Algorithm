#pragma once
#include "graph.h"

struct FlowNode;
struct Pipe;
class FlowNetwork;


struct FlowNode : Vertex<Pipe> {};


struct Pipe : UndirectedEdge<FlowNode> {
    Pipe(FlowNode *from=nullptr, FlowNode *to=nullptr, int capacity=0, int flow=0);

    const int capacity;
    int flow;
};


// FlowNetwork is actually a directed graph, but each edge has a corresponding 
// reverse edge, so it can be implemented as an undirected graph.
class FlowNetwork : public UndirectedGraph<FlowNode, Pipe> {
public:
    void AddVertex(FlowNode *vtx) override;
    void AddEdge(Pipe *edge) override;
    int ComputeMaxFlow(FlowNode *source, FlowNode *sink, MaxFlowSolver *solver = nullptr);
};

namespace max_flow_utils {
    inline int Residual(Pipe *edge) {
        return edge->capacity - edge->flow;
    }
    inline bool IsForward(FlowNode *cur, Pipe *edge) {
        return cur == edge->first;
    }
    inline bool IsBackward(FlowNode *cur, Pipe *edge) {
        return cur == edge->second;
    }
} // namespace max_flow_utils