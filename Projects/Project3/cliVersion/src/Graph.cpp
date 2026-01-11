#include "Graph.h"
#include <stdexcept>

Graph::Graph() {}

int Graph::ensureNode(char label) {
    auto it = label2idx.find(label);
    if (it != label2idx.end()) return it->second;
    int idx = (int)idx2label.size();
    label2idx[label] = idx;
    idx2label.push_back(label);
    adj.emplace_back();
    return idx;
}

bool Graph::hasNode(char label) const {
    return label2idx.find(label) != label2idx.end();
}

int Graph::nodeIndex(char label) const {
    auto it = label2idx.find(label);
    if (it == label2idx.end()) throw std::runtime_error("Unknown node");
    return it->second;
}

char Graph::indexLabel(int idx) const {
    return idx2label.at(idx);
}

int Graph::nodeCount() const { return (int)idx2label.size(); }

void Graph::addEdge(char a, char b, double meters, const std::string &roadName, int access) {
    int u = ensureNode(a);
    int v = ensureNode(b);
    Edge e1{v, meters, roadName, access};
    Edge e2{u, meters, roadName, access};
    adj[u].push_back(e1);
    adj[v].push_back(e2);
}

const std::vector<Edge>& Graph::neighbors(int idx) const {
    return adj.at(idx);
}

double Graph::edgeDistance(int u, int v) const {
    for (auto &e : adj.at(u)) if (e.to == v) return e.dist;
    return -1.0;
}
