#pragma once
#include "Parser.h"
#include <vector>
#include <string>
#include <unordered_set>

struct PlanStep {
    std::string mode; // "walk", "bike", "car", "bus"
    std::vector<char> path; // sequence of points for this step
    std::string note; // e.g., bus number
    double distance; // meters
    double time; // minutes
};

struct Plan {
    std::vector<PlanStep> steps;
    double totalDistance;
    double totalTime;
};

// speeds in m/min
struct Speeds {
    double walk = 70.0;
    double bike = 250.0;
    double bus = 400.0;
    double car = 750.0;
};

class PathFinder {
public:
    PathFinder(const DataBundle &d);
    // compute shortest path distances and predecessors with access control
    // identityFlag: bit0 (1)=FDU allowed, bit1 (2)=SHUTCM allowed
    // for carMode: if true, cars are not allowed on campus edges (access 1 or 2)
    void dijkstra(int src, std::vector<double> &dist, std::vector<int> &prev, int identityFlag, bool carMode=false) const;
    // variant of dijkstra that accepts banned edges/nodes (for Yen's k-shortest)
    void dijkstra(int src, std::vector<double> &dist, std::vector<int> &prev, int identityFlag, bool carMode, const std::unordered_set<unsigned long long> &bannedEdges, const std::unordered_set<int> &bannedNodes) const;

    // Yen's algorithm for K shortest simple paths (returns list of paths as node-label vectors)
    std::vector<std::vector<char>> kShortestPaths(char s, char t, int K, int identityFlag, bool carMode=false) const;
    std::vector<char> reconstructPath(const std::vector<int> &prev, int dst) const;

    // helpers
    Plan walkingPlan(char s, char t, int identityFlag) const;
    Plan vehiclePlanFromPath(const std::vector<char> &path, const std::string &mode) const; // bike/car time calculation on same path
    Plan vehiclePlan(char s, char t, int identityFlag, const std::string &mode) const; // bike/car full planning
    std::vector<Plan> busPlans(char s, char t, int identityFlag) const;
    double distanceBetweenNodes(char a, char b, int identityFlag) const;

    // format helpers
    // given a node sequence, produce a collapsed road-name description and total distance
    std::pair<double, std::vector<std::string>> describePathWithRoads(const std::vector<char> &path) const;
    // helpers to compute distance/time/permissions for a given node path
    double pathDistance(const std::vector<char> &path) const;
    double timeForPath(const std::vector<char> &path, const std::string &mode) const;
    bool pathCarAllowed(const std::vector<char> &path) const;
    // compute combined car-to-public-node + walk-into-campus plan when car cannot reach destination directly
    Plan carThenWalkToCampus(char s, char t, int identityFlag) const;

    std::vector<Plan> generateCandidates(char s, char t) const;

private:
    const DataBundle &data;
    Speeds sp;
};
