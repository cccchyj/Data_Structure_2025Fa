#pragma once
#include <vector>
#include <string>
#include <unordered_map>

struct Edge {
    int to;
    double dist; // meters
    std::string roadName;
    int access; // 0 public, 1 FDU, 2 SHUTCM
};

class Graph {
public:
    Graph();
    bool hasNode(char label) const;
    int nodeIndex(char label) const;
    char indexLabel(int idx) const;
    int nodeCount() const;
    void addEdge(char a, char b, double meters, const std::string &roadName, int access);
    const std::vector<Edge>& neighbors(int idx) const;
    double edgeDistance(int u, int v) const; // return meters or -1 if not connected

private:
    std::unordered_map<char,int> label2idx;
    std::vector<char> idx2label;
    std::vector<std::vector<Edge>> adj;
    int ensureNode(char label);
};
