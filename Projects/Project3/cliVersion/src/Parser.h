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
    Graph graph;
    std::vector<BusRoute> busRoutes;
    std::vector<char> busStops;
    std::unordered_map<char,std::string> busStopNames;
    // mapping from point label to pair of road names at the intersection
    std::unordered_map<char, std::pair<std::string,std::string>> pointNames;
};

// parse files in the working directory: edgeNamed.csv, busEdge.csv, busStop.csv
DataBundle parseData(const std::string &dirPath, bool publicOnly=true);
