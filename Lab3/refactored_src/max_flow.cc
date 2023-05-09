

#include <queue>
#include <unordered_map>
#include <algorithm>

#include "max_flow.h"
#include "flow_network.h"


// =========================================================================================================
// =========================================== MaxFlowSolver ===========================================
// =========================================================================================================

int MaxFlowSolver::operator()(FlowNode *source, FlowNode *sink, FlowNetwork *network) {
    return solve(source, sink, network);
}
int MaxFlowSolver::solve(FlowNode *source, FlowNode *sink, FlowNetwork *network) {
    for (auto *edge : network->edges()) {
        edge->flow = 0;
    }

    int max_flow = 0;
    std::vector<Pipe *> path;
    auto residual_less = [&](Pipe *edge1, Pipe *edge2) {
        return max_flow_utils::Residual(edge1) < max_flow_utils::Residual(edge2);
    };
    while (FindAugmentingPath(source, sink, network, path)) {
        Pipe *const bottleneck = *min_element(path.begin(), path.end(), residual_less);
        UpdateResidualGraph(source, sink, path, max_flow_utils::Residual(bottleneck));
        max_flow += max_flow_utils::Residual(bottleneck);
    }
    return max_flow;
}
bool MaxFlowSolver::UpdateResidualGraph(FlowNode *source, FlowNode *sink, std::vector<Pipe *> &path, int bottleneck) {
    FlowNode *cur = source;
    for (auto *edge : path) {
        if (max_flow_utils::IsForward(cur, edge)) {
            edge->flow += bottleneck;
            cur = edge->second;
        }
        else { // IsBackward(cur, edge)
            edge->flow -= bottleneck;
            cur = edge->first;
        }
    }
    return cur != sink; // When exiting normally, "cur" should be equal to "sink"
}



// =========================================================================================================
// ============================================= FordFulkerson =============================================
// =========================================================================================================

bool FordFulkerson::FindAugmentingPath(FlowNode *source, FlowNode *sink, FlowNetwork *network, std::vector<Pipe *> &path) {
    for (auto *vtx : network->vertices()) {
        vtx->visited = 0;
    }
    path.clear();
    return DFS(source, sink, path);
}
bool FordFulkerson::DFS(FlowNode *cur, FlowNode *sink, std::vector<Pipe *> &path) {
    cur->visited = true;
    if (cur == sink) {
        return true;
    }
    for (auto *edge : cur->edges) {
        FlowNode *next = edge->second;
        if (!next->visited && max_flow_utils::Residual(edge) > 0) {
            path.push_back(edge);
            if (DFS(next, sink, path)) {
                return true;
            }
            path.pop_back();
        }
    }
    return false;
}



// =========================================================================================================
// ============================================== EdmondsKarp ==============================================
// =========================================================================================================

bool EdmondsKarp::FindAugmentingPath(FlowNode *source, FlowNode *sink, FlowNetwork *network, std::vector<Pipe *> &path) {
    for (auto *vtx : network->vertices()) {
        vtx->visited = 0;
    }
    path.clear();
    return BFS(source, sink, path);
}
bool EdmondsKarp::BFS(FlowNode *source, FlowNode *sink, std::vector<Pipe *> &path) {
    std::queue<FlowNode *> q;
    q.push(source);
    source->visited = true;
    std::unordered_map<FlowNode *, Pipe *> back_edge;

    while (1) {
        if (q.empty()) {
            return false;
        }
        else if (q.front() == sink) {
            break;
        }
        
        FlowNode *const vtx = q.front();
        q.pop();
        for (auto *edge : vtx->edges) {
            FlowNode *next = max_flow_utils::IsForward(vtx, edge) ? edge->second : edge->first;
            if (!next->visited && max_flow_utils::Residual(edge) > 0) {
                q.push(next);
                next->visited = true;
                back_edge[next] = edge;
            }
        }
    }

    FlowNode *cur = sink;
    while (cur != source) {
        Pipe *const edge = back_edge[cur];
        path.push_back(edge);
        cur = max_flow_utils::IsForward(cur, edge) ? edge->second : edge->first;
    }
    std::reverse(path.begin(), path.end());
    return true;
}



// =========================================================================================================
// ================================================= Dinic =================================================
// =========================================================================================================

bool Dinic::FindAugmentingPath(FlowNode *source, FlowNode *sink, FlowNetwork *network, std::vector<Pipe *> &path) {
    for (auto *vtx : network->vertices()) {
        vtx->visited = 0;
    }
    path.clear();
    BFS(source, sink);              // Levelize vertices
    return DFS(source, sink, path); // Find augmenting path
}
void Dinic::BFS(FlowNode *source, FlowNode *sink) {
    // TODO
}
bool Dinic::DFS(FlowNode *cur, FlowNode *sink, std::vector<Pipe *> &path) {
    // TODO
}
// int Dinic::operator()(FlowNode *source, FlowNode *sink, FlowNetwork *network) {
//     network_ = const_cast<FlowNetwork *>(network);

//     int max_flow = 0;
//     std::vector<int> next_edge(network->vertices().size(), 0);
//     while (LevelizeVertices(source, sink)) {
//         fill(next_edge.begin(), next_edge.end(), 0);
//         int flow;
//         do {
//             flow = DFS(source, sink, numeric_limits<int>::max());
//             max_flow += flow;
//         } while (flow > 0);
//     }

//     network_ = nullptr;

//     return max_flow;
// }
// bool Dinic::LevelizeVertices(FlowNode *source, FlowNode *sink) {
//     for (auto *v : network_->vertices()) {
//         v->visited = -1; // visited means "level" in class Dinic
//     }
//     BFS(source, sink);
//     return sink->visited != -1;
// }
// void Dinic::BFS(FlowNode *source, FlowNode *sink) {
//     source->visited = 0;

//     queue<FlowNode *> q;
//     q.push(source);

//     while (!q.empty()) {
//         FlowNode *cur = q.front();
//         q.pop();

//         for (auto *e : cur->edges) {
//             FlowNode *next = e->second;
//             if (next->visited == -1 && max_flow_utils::Residual(e) > 0) {
//                 next->visited = cur->visited + 1;
//                 q.push(next);
//             }
//         }
//     }
// }
// int Dinic::DFS(FlowNode *cur, FlowNode *sink, int cur_flow) {
//     // if flow == capacity，則只存在路徑 v->first
//     // elif flow == 0，則只存在路徑 u->second
//     // else 存在路徑 u->v、v->first

//     if (cur == sink) {
//         return cur_flow;
//     }

//     for (int &i = next_edge[cur->id]; i < cur->edges.size(); i++) {
//         auto *e = cur->edges[i];
//         auto *next = e->second;
//         if (next->visited == cur->visited + 1 && max_flow_utils::Residual(e) > 0) {
//             int flow = DFS(next, sink, min(cur_flow, max_flow_utils::Residual(e)));
//             if (flow > 0) {
//                 e->flow += flow;
//                 return flow;
//             }
//         }
//     }

//     return 0;
// }