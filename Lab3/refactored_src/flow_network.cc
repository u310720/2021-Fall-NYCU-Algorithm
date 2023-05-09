#include "flow_network.h"
#include "max_flow.h"

Pipe::Pipe(FlowNode *from, FlowNode *to, int capacity, int flow) 
    : UndirectedEdge<FlowNode>(from, to), capacity(capacity), flow(flow) {}


void FlowNetwork::AddVertex(FlowNode *vtx) {
    vertices_.push_back(vtx);
}
void FlowNetwork::AddEdge(Pipe *edge) {
    edges_.push_back(edge);
    edge->first->edges.push_back(edge);
    edge->second->edges.push_back(edge);
}
int FlowNetwork::ComputeMaxFlow(FlowNode *source, FlowNode *sink, MaxFlowSolver *solver) {
    return solver ? solver->solve(source, sink, this) : FordFulkerson().solve(source, sink, this);
}