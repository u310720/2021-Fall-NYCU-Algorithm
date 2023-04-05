#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <climits>
using namespace std;

struct Horizontal // interval tree sorted by the left boundary
{
    int left, right;
    set<int> id;
    map<int, Horizontal> HRight; // sub-interval tree sorted by the right boundary
};
struct Vertical // interval tree sorted by the lower boundary
{
    int low, up;
    set<int> id;
    map<int, Vertical> VUp; // sub-interval tree sorted by the upper boundary
};
struct Interval
{
    map<int, Horizontal> HLeft;
    map<int, Vertical> VLow;
};

void OperationI(istringstream &iss, Interval *I)
{
    string _id, _left, _bot, _right, _top;
    iss >> _id >> _left >> _bot >> _right >> _top;
    _id.erase(_id.begin()); // remove 'r'
    int id = stoi(_id), left = stoi(_left), low = stoi(_bot), right = stoi(_right), up = stoi(_top);

    auto itL = I->HLeft.find(left);
    if (itL != I->HLeft.end()) // have the same left boundry
    {
        if (right == itL->second.right) // have the same right boundry, that is, have the same interval
            itL->second.id.insert(id);
        else // process the sub-interval tree sorted by the right boundary
        {
            auto itR = itL->second.HRight.find(right);
            if (itR != itL->second.HRight.end()) // have the same right boundry(not equal to itL's right), add new id in
                itR->second.id.insert(id);
            else // it's a new interval
            {
                set<int> IDtmp = {id};
                Horizontal Htmp = {left, right, IDtmp};
                itL->second.HRight.insert(make_pair(right, Htmp));
            }
        }
    }
    else // it's a new interval
    {
        set<int> IDtmp = {id};
        Horizontal Htmp = {left, right, IDtmp};
        I->HLeft.insert(make_pair(left, Htmp));
    }

    auto itLo = I->VLow.find(low);
    if (itLo != I->VLow.end()) // have the same lower boundry
    {
        if (up == itLo->second.up) // have the same upper boundry, that is, have the same interval
            itLo->second.id.insert(id);
        else // process the sub-interval tree sorted by the upper boundary
        {
            auto itU = itLo->second.VUp.find(up);
            if (itU != itLo->second.VUp.end()) // have the same upper boundry(not equal to itLo's up), add new id in
                itU->second.id.insert(id);
            else // it's a new interval
            {
                set<int> IDtmp = {id};
                Vertical Htmp = {low, up, IDtmp};
                itLo->second.VUp.insert(make_pair(up, Htmp));
            }
        }
    }
    else // it's a new interval
    {
        set<int> IDtmp = {id};
        Vertical Htmp = {low, up, IDtmp};
        I->VLow.insert(make_pair(low, Htmp));
    }
}
void OperationD(istringstream &iss, Interval *I)
{
    string _left, _bot, _right, _top;
    iss >> _left >> _bot >> _right >> _top;
    int left = stoi(_left), low = stoi(_bot), right = stoi(_right), up = stoi(_top);

    set<int> id_H;
    auto itL = I->HLeft.find(left), itR = itL->second.HRight.find(right);
    if (itL != I->HLeft.end()) // the left boundry exist
    {
        if (right == itL->second.right) // found the interval to delete
            for (const auto &itID : itL->second.id)
                id_H.insert(itID);
        else if (itR != itL->second.HRight.end())
            for (const auto &itID : itR->second.id)
                id_H.insert(itID);
    }
    set<int> id_V;
    auto itLo = I->VLow.find(low), itU = itLo->second.VUp.find(up);
    if (itLo != I->VLow.end()) // the lower boundry exist
    {
        if (up == itLo->second.up) // found the interval to delete
            for (const auto &itID : itLo->second.id)
                id_V.insert(itID);
        else if (itU != itLo->second.VUp.end())
            for (const auto &itID : itU->second.id)
                id_V.insert(itID);
    }
    int delete_id;
    for (const auto &it : id_H) // find the smallest id in the intersection of two sets
    {
        if (id_V.find(it) != id_V.end())
        {
            delete_id = it;
            break;
        }
    }

    if (itL->second.id.find(delete_id) != itL->second.id.end()) // found the id to be delete
    {
        itL->second.id.erase(delete_id);
        if (itL->second.id.empty())
        {
            if (itL->second.HRight.empty()) // there is no interval here
                I->HLeft.erase(itL);
            else // let HRight's first element take the place of this node
            {
                itR = itL->second.HRight.begin();
                itL->second.id = itR->second.id;
                itL->second.right = itR->second.right;
                itL->second.HRight.erase(itR);
            }
        }
    }
    else // find the right boundry in the sub-interval tree
    {
        if (itR != itL->second.HRight.end()) // the right boundry exist, found the interval to delete
        {
            itR->second.id.erase(delete_id);
            if (itR->second.id.empty()) // there is no interval here
                itL->second.HRight.erase(itR);
        }
    }
    if (itLo->second.id.find(delete_id) != itLo->second.id.end()) // found the id to be delete
    {
        itLo->second.id.erase(delete_id);
        if (itLo->second.id.empty())
        {
            if (itLo->second.VUp.empty()) // there is no interval here
                I->VLow.erase(itLo);
            else // let VUp's first element take the place of this node
            {
                itU = itLo->second.VUp.begin();
                itLo->second.id = itU->second.id;
                itLo->second.up = itU->second.up;
                itLo->second.VUp.erase(itU);
            }
        }
    }
    else // find the upper boundry in the sub-interval tree
    {
        if (itU != itLo->second.VUp.end()) // the upper boundry exist, found the interval to delete
        {
            itU->second.id.erase(delete_id);
            if (itU->second.id.empty()) // there is no interval here
                itLo->second.VUp.erase(itU);
        }
    }
}
void OperationO(ofstream &ofs, istringstream &iss, Interval *I)
{
    enum _violet
    {
        legal,
        violate
    };

    ofs << "O" << endl;
    string _left, _bot, _right, _top;
    iss >> _left >> _bot >> _right >> _top;
    int left = stoi(_left), low = stoi(_bot), right = stoi(_right), up = stoi(_top);

    map<int, bool> id_violate_H;
    for (const auto &itL : I->HLeft)
    {
        if (itL.second.left > left)
            break;
        if (right <= itL.second.right)
        {
            if (left - itL.second.left < 20 || itL.second.right - right < 20)
                for (const auto &itID : itL.second.id)
                    id_violate_H.insert(make_pair(itID, violate));
            else
                for (const auto &itID : itL.second.id)
                    id_violate_H.insert(make_pair(itID, legal));
        }
        for (const auto &itR : itL.second.HRight)
        {
            if (right <= itR.second.right)
            {
                if (left - itR.second.left < 20 || itR.second.right - right < 20)
                    for (const auto &itID : itR.second.id)
                        id_violate_H.insert(make_pair(itID, violate));
                else
                    for (const auto &itID : itR.second.id)
                        id_violate_H.insert(make_pair(itID, legal));
            }
        }
    }
    map<int, bool> id_violate_V;
    for (const auto &itLo : I->VLow)
    {
        if (itLo.second.low > low)
            break;
        if (up <= itLo.second.up)
        {
            if (low - itLo.second.low < 20 || itLo.second.up - up < 20)
                for (const auto &itID : itLo.second.id)
                    id_violate_V.insert(make_pair(itID, violate));
            else
                for (const auto &itID : itLo.second.id)
                    id_violate_V.insert(make_pair(itID, legal));
        }
        for (const auto &itU : itLo.second.VUp)
        {
            if (up <= itU.second.up)
            {
                if (low - itU.second.low < 20 || itU.second.up - up < 20)
                    for (const auto &itID : itU.second.id)
                        id_violate_V.insert(make_pair(itID, violate));
                else
                    for (const auto &itID : itU.second.id)
                        id_violate_V.insert(make_pair(itID, legal));
            }
        }
    }
    auto itH = id_violate_H.begin(), itV = id_violate_V.begin();
    while (itH != id_violate_H.end() && itV != id_violate_V.end())
    {
        if (itH->first > itV->first)
            itV++;
        else if (itH->first < itV->first)
            itH++;
        else
        {
            if (itH->second == legal && itV->second == legal)
                ofs << "r" << itH->first << endl;
            else
                ofs << "r" << itH->first << " violate" << endl;
            itH++;
            itV++;
        }
    }
}
void OperationS(ofstream &ofs, istringstream &iss, Interval *I)
{
    ofs << "S" << endl;
    string _left, _bot, _right, _top;
    iss >> _left >> _bot >> _right >> _top;
    int left = stoi(_left), low = stoi(_bot), right = stoi(_right), up = stoi(_top);

    set<int> id_H;
    auto itL = I->HLeft.find(left);
    if (itL != I->HLeft.end()) // the left boundry exist
    {
        if (right == itL->second.right) // found the interval
            for (const auto &it : itL->second.id)
                id_H.insert(it);
        else
        {
            auto itR = itL->second.HRight.find(right);
            if (itR != itL->second.HRight.end()) // the left boundry exist
                for (const auto &it : itR->second.id)
                    id_H.insert(it);
        }
    }
    set<int> id_V;
    auto itLo = I->VLow.find(low);
    if (itLo != I->VLow.end()) // the lower boundry exist
    {
        if (up == itLo->second.up) // found the interval
            for (const auto &it : itLo->second.id)
                id_V.insert(it);
        else
        {
            auto itU = itLo->second.VUp.find(up);
            if (itU != itLo->second.VUp.end()) // the upper boundry exist
                for (const auto &it : itU->second.id)
                    id_V.insert(it);
        }
    }
    auto itH = id_H.begin(), itV = id_V.begin();
    while (itH != id_H.end() && itV != id_V.end())
    {
        if (*itH > *itV)
            itV++;
        else if (*itH < *itV)
            itH++;
        else
        {
            ofs << "r" << *itH << endl;
            itH++;
            itV++;
        }
    }
}
void OperationA(ofstream &ofs, istringstream &iss, Interval *I)
{
    ofs << "A" << endl;
    map<int, pair<int, int>> id_w_h;
    map<int, int> id_area;
    string _left, _low;
    iss >> _left >> _low;
    int left = stoi(_left), low = stoi(_low);

    auto itL = I->HLeft.find(left);
    if (itL != I->HLeft.end()) // process width
    {
        for (const auto &it : itL->second.id)
            id_w_h.insert(make_pair(it, make_pair(itL->second.right - left, 0)));
        for (const auto &itR : itL->second.HRight)
            for (const auto &it : itR.second.id)
                id_w_h.insert(make_pair(it, make_pair(itR.second.right - left, 0)));
    }
    else
        return;
    auto itLo = I->VLow.find(low);
    if (itLo != I->VLow.end()) // process height
    {
        for (const auto &it : itLo->second.id)
            id_w_h.find(it)->second.second = itLo->second.up - low;
        for (const auto &itU : itLo->second.VUp)
            for (const auto &it : itU.second.id)
                id_w_h.find(it)->second.second = itU.second.up - low;
    }
    else
        return;

    int MinArea = INT_MAX;
    for (const auto &it : id_w_h) // calculate area
    {
        int area = it.second.first * it.second.second;
        MinArea = area < MinArea && area != 0 ? area : MinArea;
        id_area.insert(make_pair(it.first, area));
    }
    for (const auto &it : id_area)
        if (it.second == MinArea)
            ofs << "r" << it.first << endl;
}

int main(int argc, char **argv)
{
    string fin = argc < 2 ? "case2.txt" : argv[1];
    string fout = argc < 3 ? "output.txt" : argv[2];
    ifstream ifs;
    ofstream ofs;
    ifs.open(fin);
    ofs.open(fout);
    if (!ifs || !ofs)
    {
        cerr << "File could not open!" << endl;
        return 1;
    }

    string line;
    Interval I;
    while (!ifs.eof())
    {
        getline(ifs, line);

        /*if (line == "S 75 944 453 968")
            cout << endl; // DEBUG*/

        istringstream iss(line);
        string operation;
        iss >> operation; // read operation
        if (operation == "I")
            OperationI(iss, &I);
        else if (operation == "D")
            OperationD(iss, &I);
        else if (operation == "O")
            OperationO(ofs, iss, &I);
        else if (operation == "S")
            OperationS(ofs, iss, &I);
        else if (operation == "A")
            OperationA(ofs, iss, &I);
    }
    ifs.close();
    ofs.close();
    return 0;
}