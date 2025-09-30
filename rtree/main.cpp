#include <string>
#include <vector>
#include <iostream>
#include "RTree.h"

using namespace std;

void print(const int& command, vector<vector<vector<pair<int, int>>>>& objects_n, string& output) {
    output.clear();
    output = to_string(command);

    switch (command)
    {
    case 0: // OBJECTS
        if (objects_n.empty()) {
            output += "|0|END";
            return;
        }
        output += "|" + to_string(objects_n[0].size());
        for (auto& polygon : objects_n[0]) {
            output += "|" + to_string(polygon.size());

            for (auto& point : polygon) {
                output += "|" + to_string(point.first) + "|" + to_string(point.second);
            }
        }
        break;

    case 1: //MBRS:
        cout << " number of (objects) :" << to_string(objects_n.size()) << endl;
        output += "|" + to_string(objects_n.size());
        for (auto& objects : objects_n) {
            output += "|" + to_string(objects.size());
            cout << " number of polygons :" << to_string(objects.size()) << endl;
            for (auto& polygon : objects) {
                output += "|" + to_string(polygon.size());
                cout << " number of points :" << to_string(polygon.size()) << endl;
                for (auto& point : polygon) {
                    output += "|" + to_string(point.first) + "|" + to_string(point.second);
                    cout << "   point:" << to_string(point.first) << " | " << to_string(point.second) << endl;
                }
            }
            cout << endl << "--------------------" << endl;
        }
        break;

    default:
        output += "|0";
        break;
    }

    output += "|END";
}

void print_pair(vector<pair<int, int>> output) {
    for (auto& x : output) {
        cout << " ( " << x.first << " , " << x.second << " ) ";
    }
}

int main(int argc, char* argv[])
{
    vector<vector<pair<int, int>>> vpoints;

    //   First Test
    float coord[16] = { 20, 59, 20, 43, 50, 58 , 48,67, 105, 68, 74, 64, 83, 40, 104, 54 };

    vector<pair<int, int>> points;
    for (int i = 0; i < 16; i += 2) {
        pair<int, int> A;
        A.first = coord[i];
        A.second = coord[i + 1];
        points.push_back(A);
    }
    for (unsigned int i = 0; i < 8; i += 2) {
        vector<pair<int, int>> sub1(points.begin() + i, points.begin() + i + 2);
        vpoints.push_back(sub1);
    }

    float coord2[12] = { 12, 28, 19,15, 40,29, 69,25, 70,28, 60,15 };

    vector<pair<int, int>> points2;
    for (int i = 0; i < 12; i += 2) {
        pair<int, int> A;
        A.first = coord2[i];
        A.second = coord2[i + 1];
        points2.push_back(A);
    }
    for (unsigned int i = 0; i < 6; i += 3) {
        vector<pair<int, int>> sub1(points2.begin() + i, points2.begin() + i + 3);
        vpoints.push_back(sub1);
    }

    RTree rtree;

    string output;
    vector<vector<pair<int, int>>> objects;
    vector<vector<vector<pair<int, int>>>> objects_n;

    for (auto& x : vpoints)
    {
        cout << "inserting " << x.size() << ": ";
        print_pair(x);
        Rect rect = rtree.MBR(x);
        rtree.Insert(rect.m_min, rect.m_max, x);
        cout << endl;
    }

    rtree.getMBRs(objects_n);
    print(1, objects_n, output);

    //   Second Test
    cout << " INSERTING ONE MORE " << endl;
    vector<pair<int, int>> ad;
    pair<int, int> ad1;
    ad1.first = 54;
    ad1.second = 12;
    pair<int, int> ad2;
    ad2.first = 53;
    ad2.second = 4;
    ad.push_back(ad2);
    ad.push_back(ad1);

    cout << "inserting " << ad.size() << ": ";
    print_pair(ad);
    Rect rect = rtree.MBR(ad);
    rtree.Insert(rect.m_min, rect.m_max, ad);
    cout << endl;

    rtree.getMBRs(objects_n);
    print(1, objects_n, output);

    cout << " REMOVING LAST INSERTED " << endl;
    print_pair(ad);
    rtree.Remove(rect.m_min, rect.m_max, ad);
    cout << endl;

    rtree.getMBRs(objects_n);
    print(1, objects_n, output);



    cout << " BUSQUEDA EN RECTANGULO (10,10)-(60,60)" << endl;
    Rect query(10, 10, 60, 60);
    vector<vector<pair<int, int>>> found;
    rtree.Search(query, found);

    cout << "Encontrados: " << found.size() << " objetos" << endl;
    for (auto& poly : found) {
        for (auto& p : poly) {
            cout << "(" << p.first << "," << p.second << ") ";
        }
        cout << endl;
    }

    return 0;
}
