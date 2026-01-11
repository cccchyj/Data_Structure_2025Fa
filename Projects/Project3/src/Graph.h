#pragma once
#include <vector>
#include <string>
#include <unordered_map>

struct Edge {
    int to;                    // Target node index
    double dist;               // Distance in meters
    std::string roadName;      // Road name for description
    int access;                // Access control: 0=public, 1=FDU, 2=SHUTCM
};

class Graph {
public:
    Graph();
    bool hasNode(char label) const;            // Check if node exists
    int nodeIndex(char label) const;
    char indexLabel(int idx) const;
    int nodeCount() const;
    void addEdge(char a, char b, double meters, const std::string &roadName, int access);
    const std::vector<Edge>& neighbors(int idx) const;
    double edgeDistance(int u, int v) const; // return meters or -1 if not connected

private:
    std::unordered_map<char,int> label2idx;     // Map node labels (A-Z) to indices
    std::vector<char> idx2label;                // Map indices back to labels
    std::vector<std::vector<Edge>> adj;         // Adjacency list
    int ensureNode(char label);                 // Helper to add nodes if missing
};
