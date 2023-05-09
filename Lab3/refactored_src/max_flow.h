#pragma once

#include <vector>

class FlowNode;
class Pipe;
class FlowNetwork;

class MaxFlowSolver {
public:
    int operator()(FlowNode *source, FlowNode *sink, FlowNetwork *network);
    int solve(FlowNode *source, FlowNode *sink, FlowNetwork *network);

protected:
    virtual bool FindAugmentingPath(FlowNode *source, FlowNode *sink, FlowNetwork *network, std::vector<Pipe *> &path) = 0;
    
    bool UpdateResidualGraph(FlowNode *source, FlowNode *sink, std::vector<Pipe *> &path, int bottleneck);
    
};

// Time complexity = O(E * f), f is max flow
class FordFulkerson final : public MaxFlowSolver {
private:
    bool FindAugmentingPath(FlowNode *source, FlowNode *sink, FlowNetwork *network, std::vector<Pipe *> &path) override;
    bool DFS(FlowNode *cur, FlowNode *sink, std::vector<Pipe *> &path);
};

// Time complexity = O(V * E^2)
class EdmondsKarp final : public MaxFlowSolver {
private:
    bool FindAugmentingPath(FlowNode *source, FlowNode *sink, FlowNetwork *network, std::vector<Pipe *> &path) override;
    bool BFS(FlowNode *source, FlowNode *sink, std::vector<Pipe *> &path);
};

// Time complexity = O(V^2 * E)
class Dinic final : public MaxFlowSolver {
private:
    bool FindAugmentingPath(FlowNode *source, FlowNode *sink, FlowNetwork *network, std::vector<Pipe *> &path) override;
    void BFS(FlowNode *source, FlowNode *sink);
    bool DFS(FlowNode *cur, FlowNode *sink, std::vector<Pipe *> &path);
};