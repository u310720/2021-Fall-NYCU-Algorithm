#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdlib.h> //abs()
using namespace std;

int numV, numE, numT;

struct pin
{
    int x, y;
    pin(int _id, int _x, int _y) : x(_x), y(_y) {}
};
vector<pin> V; // all pins

inline int dist(int i, int j) { return abs(V[i].x - V[j].x) + abs(V[i].y - V[j].y); }
struct edge
{
    int p1, p2; // connected points' index
    int d;      // |x1 - x2| + |y1 - y2|
    edge(int _p1, int _p2) : p1(_p1), p2(_p2), d(dist(_p1, _p2)) {}
    bool operator<(const edge &e) const { return d < e.d; }
};
vector<edge> E; // all edges

struct disjoint_set
{
    vector<int> ds;
    disjoint_set(int numV)
    {
        for (int i = 0; i < numV; i++)
            ds.push_back(i);
    }
    int find(int x) { return x == ds[x] ? x : (ds[x] = find(ds[x])); }
    void merge(int x, int y) { ds[find(y)] = find(x); }
};

disjoint_set read(string &inF)
{
    ifstream ifs;
    string s;
    ifs.open(inF);
    if (!ifs)
    {
        cerr << "Can't read." << endl;
        // return; // change to throw
    }

    ifs >> s; // # of pins
    numT = numV = stoi(s);
    disjoint_set DS(numV);

    for (int i = 0; i < numV; i++) // read pins
    {
        int x, y;
        ifs >> s;
        x = stoi(s);
        ifs >> s;
        y = stoi(s);
        pin p(i, x, y);
        V.push_back(p);
    }
    while (1) // read exist routing and update disjoint-set
    {
        int i, j;
        ifs >> s;
        i = stoi(s);
        ifs >> s;
        j = stoi(s);
        if (ifs.eof())
            break;
        DS.merge(i, j);
        numT--;
    }
    ifs.close();

    return DS;
}

void buildE()
{
    numE = 0;
    for (int i = 0; i < numV - 1; i++)
    {
        for (int j = i + 1; j < numV; j++)
        {
            edge e(i, j);
            E.push_back(e);
            numE++;
        }
    }
    sort(E.begin(), E.end());
}

vector<edge *> kruskal(disjoint_set &DS)
{
    vector<edge *> addE;
    for (int i = 0, j = 0; i < numT; i++, j++)
    {
        while (DS.find(E[j].p1) == DS.find(E[j].p2))
            if (j < numE - 1)
                j++;
            else
                return addE;

        DS.merge(E[j].p1, E[j].p2);
        addE.push_back(&E[j]);
    }

    return addE;
}

void output(string &outF, vector<edge *> &addE)
{
    ofstream ofs;
    ofs.open(outF);

    int cost = 0;
    for (const auto &it : addE)
        cost += it->d;
    ofs << cost << endl;

    for (const auto &it : addE)
        ofs << it->p1 << " " << it->p2 << endl;

    ofs.close();
}

int main(int argc, char **argv)
{
    string inF = argc == 3 ? argv[1] : "case2.txt";
    string outF = argc == 3 ? argv[2] : "output.txt";

    disjoint_set DS = read(inF);
    buildE();
    vector<edge *> addE = kruskal(DS);
    output(outF, addE);

    return 0;
}