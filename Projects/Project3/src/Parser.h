#pragma once
#include "Graph.h"
#include <string>
#include <vector>
#include <unordered_map>

struct BusRoute {
    std::string busNo;
    std::vector<char> seq; // sequence of points this bus goes through
};

struct DataBundle {
    Graph graph;                              // Road network graph
    std::vector<BusRoute> busRoutes;          // Bus route information
    std::vector<char> busStops;               // List of bus stop nodes
    std::unordered_map<char,std::string> busStopNames;  // Bus stop names
    std::unordered_map<char, std::pair<std::string,std::string>> pointNames;  // Node names
};

// parse files in the working directory: edgeNamed.csv, busEdge.csv, busStop.csv
DataBundle parseData(const std::string &dirPath, bool publicOnly=true);
