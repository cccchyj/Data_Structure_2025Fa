#include "Parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>

static std::vector<std::string> splitCSVLine(const std::string &line) {
    std::vector<std::string> out;
    std::string cur;
    std::istringstream in(line);
    while (std::getline(in, cur, ',')) {
        // trim
        size_t a = cur.find_first_not_of(" \t\r\n");
        size_t b = cur.find_last_not_of(" \t\r\n");
        if (a==std::string::npos) out.push_back(""); else out.push_back(cur.substr(a, b-a+1));
    }
    return out;
}

DataBundle parseData(const std::string &dirPath, bool publicOnly) {
    namespace fs = std::filesystem;
    DataBundle data;
    fs::path base(dirPath);
    fs::path edgeNamed = base / "edgeNamed.csv";
    std::ifstream fin(edgeNamed.string());
    if (!fin) throw std::runtime_error(std::string("Cannot open ") + edgeNamed.string());
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        auto toks = splitCSVLine(line);
        if (toks.size() < 5) continue;
        char a = toks[0][0];
        char b = toks[1][0];
        double hundredMeters = std::stod(toks[2]);
        double meters = hundredMeters * 100.0;
        std::string road = toks[3];
        int access = std::stoi(toks[4]);
        if (publicOnly && access != 0) continue;
        data.graph.addEdge(a,b,meters,road,access);
    }
    fin.close();

    fs::path busStopFile = base / "busStop.csv";
    std::ifstream fstop(busStopFile.string());
    if (!fstop) throw std::runtime_error(std::string("Cannot open ") + busStopFile.string());
    while (std::getline(fstop, line)) {
        if (line.empty()) continue;
        auto toks = splitCSVLine(line);
        if (toks.empty()) continue;
        char p = toks[0][0];
        data.busStops.push_back(p);
        if (toks.size()>=2) data.busStopNames[p] = toks[1];
        else data.busStopNames[p] = std::string(1,p);
    }
    fstop.close();

    fs::path busEdgeFile = base / "busEdge.csv";
    std::ifstream fbus(busEdgeFile.string());
    if (!fbus) throw std::runtime_error(std::string("Cannot open ") + busEdgeFile.string());
    while (std::getline(fbus, line)) {
        if (line.empty()) continue;
        // some lines have trailing commas
        auto toks = splitCSVLine(line);
        if (toks.size() < 2) continue;
        BusRoute br;
        br.busNo = toks[0];
        for (size_t i=1;i<toks.size();++i) {
            if (toks[i].empty()) continue;
            char p = toks[i][0];
            br.seq.push_back(p);
        }
        if (!br.seq.empty()) data.busRoutes.push_back(br);
    }
    fbus.close();

    // parse pointNamed.csv to get human-readable road names at points
    fs::path pointNamedFile = base / "pointNamed.csv";
    std::ifstream fpoint(pointNamedFile.string());
    if (!fpoint) {
        // Not fatal: some datasets may omit it
    } else {
        while (std::getline(fpoint, line)){
            if (line.empty()) continue;
            auto toks = splitCSVLine(line);
            if (toks.size() < 3) continue;
            char p = toks[0][0];
            data.pointNames[p] = {toks[1], toks[2]};
        }
        fpoint.close();
    }

    return data;
}
