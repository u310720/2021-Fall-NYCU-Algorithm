#include <iostream>
#include <fstream>
#include <vector>
#include <map>     // for parser
#include <queue>   // for dijkstra: priority quene
#include <climits> // for INT_MAX
using namespace std;

#define Index second

struct person
{
    vector<int> nTy; // nTy[0] == 3 means this person has 3 tangyuans of the first flavor

    person(int nPeople)
    {
        nTy.assign(nPeople, 0);
    }
};

struct building;
struct road
{
    building *destination;
    int distance;

    road(building *_destination, int _length) : destination(_destination), distance(_length) {}
};
struct building
{
    string name;
    int distance, sellFlavor, index; // distance & tangyuan flavor
    building *source;
    vector<road> connected;
    bool finish;

    building(string &_name, int _flavor, int _index) : name(_name), distance(INT_MAX), sellFlavor(_flavor), source(nullptr), finish(false), index(_index) {}
    bool operator<(const building &b) const { return b.distance < distance; }
};
struct inputData
{
    int nFlavor;
    int nPeople;
    int nMarket;
    vector<building> B; // B[0] is home, B[1] is party, B[2...n-1] is markets
    vector<person> P;
};
typedef vector<pair<building *, int>> path_table;

struct parser
{
    inputData *data;

    inputData *input(string &inf)
    {
        ifstream ifs(inf);

        data = new inputData;
        string tmp, _home, _party;
        int nTy;

        ifs >> tmp >> _home;
        ifs >> tmp >> _party;
        ifs >> tmp >> tmp;
        data->nFlavor = stoi(tmp);
        ifs >> tmp >> tmp;
        data->nPeople = stoi(tmp);

        for (int i = 1; i < data->nPeople; i++)
        {
            ifs >> tmp;
            nTy = stoi(tmp);
            person p(data->nFlavor);
            for (int j = 0; j < nTy; j++)
            {
                ifs >> tmp;
                p.nTy[stoi(tmp) - 1]++;
            }
            data->P.push_back(p);
        }

        map<string, int> bMap; // building name, index of data->B
        ifs >> tmp >> tmp;
        data->nMarket = stoi(tmp);
        building *home = new building(_home, -1, 0);
        data->B.push_back(*home);
        bMap.insert(make_pair(home->name, 0));
        building *party = new building(_party, -1, 1);
        data->B.push_back(*party);
        bMap.insert(make_pair(party->name, 1));
        for (int i = 2; i < data->nMarket + 2; i++)
        {
            string _name, _flavor;
            ifs >> _name >> _flavor;
            building *market = new building(_name, stoi(_flavor), i);
            data->B.push_back(*market);
            bMap.insert(make_pair(_name, i));
        }

        string from, to, _dist;
        while (ifs >> to >> from >> _dist)
        {
            road r(&data->B[bMap.find(to)->Index], stoi(_dist));
            data->B[bMap.find(from)->Index].connected.push_back(r);
        }

        ifs.close();
        return data;
    }

    void outputShortestPath(string &of)
    {
        ofstream ofs(of);

        int size = data->B.size();
        for (int i = 0; i < size; i++)
            ofs << data->B[i].name << ": " << data->B[i].distance << endl;
        ofs.close();
    }
    void outputFlavor(string &of, int flavor)
    {
        ofstream ofs(of);

        ofs << flavor << endl;
        ofs.close();
    }
};

void dijkstra(vector<building> &B)
{
    priority_queue<building> pq;

    B[0].distance = 0; // home
    pq.push(B[0]);
    while (!pq.empty())
    {
        int index = pq.top().index;
        pq.pop();

        if (B[index].finish)
            continue;

        for (auto &adj : B[index].connected)
        {
            if (B[index].distance + adj.distance < adj.destination->distance)
            {
                adj.destination->distance = B[index].distance + adj.distance;
                adj.destination->source = &B[index];
                pq.push(*adj.destination);
            }
        }

        B[index].finish = true;
    }

    for (auto &b : B)
        if (b.distance == INT_MAX) // does not exist path form home to b
            b.distance = -1;
};

struct pipe;
struct node
{
    string name;
    vector<pipe *> inflow;
    vector<pipe *> outflow;
    bool visited;

    node(string _name) : name(_name) {}
};
struct pipe
{
    node *from, *to;
    int capacity;
    pipe *twin;

    pipe(node *_from, node *_to, int _capacity) : from(_from), to(_to), capacity(_capacity)
    {
        _from->outflow.push_back(this);
        _to->inflow.push_back(this);
    }
};
struct max_flow
{
    inputData *data;
    node *s, *t;
    vector<node *> ty1; // tangyuan_layer 1
    vector<node *> ty2; // tangyuan_layer 1
    vector<node *> F;   // Karen's friends
    int KarenFlavor;

    max_flow(inputData *_data) : data(_data) {}
    bool pipeCmp(pipe *p1, pipe *p2) const { return p1->capacity < p2->capacity; }
    void createTyLayer(int nFlavor)
    {
        for (int i = 0; i < nFlavor; i++)
        {
            node *n1 = new node(to_string(i));
            node *n2 = new node(to_string(i));
            ty1.push_back(n1);
            ty2.push_back(n2);
        }
    }
    void createFriends(int nPeople)
    {
        for (int i = 0; i < nPeople - 1; i++)
        {
            node *n = new node("f" + to_string(i));
            F.push_back(n);
        }
    }
    void setKaren(vector<int> &nTy)
    {
        nTy.assign(data->nFlavor, 0);

        building *cur = &data->B[1]; // party
        while (cur->source != nullptr)
        {
            if (cur->sellFlavor != -1)
                nTy[cur->sellFlavor - 1]++;
            cur = cur->source;
        }

        KarenFlavor = 0;
        for (int i = 0; i < data->nFlavor; i++)
        {
            KarenFlavor += nTy[i] != 0 ? 1 : 0;
            if (nTy[i] > 1)
            {
                pipe *pos = new pipe(s, ty1[i], nTy[i] - 1);
                pipe *neg = new pipe(ty1[i], s, 0);
                pos->twin = neg;
                neg->twin = pos;
            }
        }
    }
    void setFriends()
    {
        pipe *pos, *neg;
        for (int i = 0; i < data->nPeople - 1; i++)
        {
            for (int j = 0; j < data->nFlavor; j++)
            {
                if (data->P[i].nTy[j] == 0)
                {
                    // ty1 <-> F
                    pos = new pipe(ty1[j], F[i], 1);
                    neg = new pipe(F[i], ty1[j], 0);
                }
                else if (data->P[i].nTy[j] > 1)
                {
                    // F <-> ty2
                    pos = new pipe(F[i], ty2[j], data->P[i].nTy[j] - 1);
                    neg = new pipe(ty2[j], F[i], 0);
                }
                else
                    continue;

                pos->twin = neg;
                neg->twin = pos;
            }
        }
    }
    void setTerminal(vector<int> &nTy)
    {
        for (int i = 0; i < data->nFlavor; i++)
            if (nTy[i] == 0)
            {
                pipe *pos = new pipe(ty2[i], t, 1);
                pipe *neg = new pipe(t, ty2[i], 0);
                pos->twin = neg;
                neg->twin = pos;
            }
    }
    void buildGraph()
    {
        s = new node("Karen");
        t = new node("terminal");
        createTyLayer(data->nFlavor);
        createFriends(data->nPeople);

        vector<int> nTy;
        setKaren(nTy);
        setFriends();
        setTerminal(nTy);
    }
    int DFS(node *cur, vector<pipe *> *path)
    {
        cur->visited = true;
        for (auto &itp : cur->outflow)
        {
            if (itp->capacity == 0)
                continue;
            else
            {
                if (itp->to->visited)
                    continue;

                if (itp->to == t)
                {
                    path->push_back(itp);
                    return EXIT_SUCCESS;
                }
            }

            path->push_back(itp);
            if (!itp->to->visited && DFS(itp->to, path) == EXIT_SUCCESS)
                return EXIT_SUCCESS;
            else
                path->pop_back();
        }
        return EXIT_FAILURE; // could not find path from s to t
    }
    vector<pipe *> *findAugPath() // find augmenting path by DFS
    {
        vector<pipe *> *path = new vector<pipe *>;

        for (auto &it : ty1)
            it->visited = false;
        for (auto &it : ty2)
            it->visited = false;
        for (auto &it : F)
            it->visited = false;
        t->visited = false;

        if (DFS(s, path) == EXIT_SUCCESS)
            return path;
        else
        {
            path->clear();
            return nullptr;
        }
    }
    void printPath(vector<pipe *> *path) // for debug
    {
        int count = 1;
        for (const auto &p : *path)
            cout << count++ << ". " << p->from->name << "->" << p->to->name << "   ";
        cout << endl;
    }
    int getMostFlavor()
    {
        buildGraph();

        vector<pipe *> *path;
        while ((path = findAugPath()) != nullptr)
        {
            // printPath(path); // for debug
            KarenFlavor++;
            for (auto &itp : *path)
            {
                itp->capacity--;
                itp->twin->capacity++;
            }
            path->clear();
        }

        return KarenFlavor;
    }
};

int main(int argc, char **argv)
{
    string inf = argc == 1 ? "case1.txt" : argv[1];
    string of1 = argc == 1 ? "sPath.txt" : argv[2];
    string of2 = argc == 1 ? "mFlow.txt" : argv[3];

    inputData *data;
    parser ps;

    data = ps.input(inf);

    dijkstra(data->B);
    ps.outputShortestPath(of1);

    max_flow mf(data);
    ps.outputFlavor(of2, mf.getMostFlavor());

    return EXIT_SUCCESS;
}