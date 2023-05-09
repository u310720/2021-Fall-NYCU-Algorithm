#include "graph.h"
#include <vector>

namespace shopping {
    struct Building : Vertex<Road> {
        int flavor, distance;
    };

    struct Road : UndirectedEdge<Building> {
        int distance;
    };

    class TangyuanMap : public ::UndirectedGraph<Building, Road> {
    public:
        void AddVertex(Building *v) override;
        void AddEdge(Road *e) override;
        std::vector<Road *> ComputeSortestPath();

    private:
        int dijkstra();
    };
}
