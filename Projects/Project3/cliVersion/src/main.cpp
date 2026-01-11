#include <iostream>
#include <string>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include "Parser.h"
#include "PathFinder.h"

#include <sstream>
#include <map>

int main(int argc, char** argv){
    namespace fs = std::filesystem;
    // Determine data directory: prefer current_working_dir/data, else try executable parent/../data
    fs::path dataDir = fs::current_path() / "data";
    if (!fs::exists(dataDir)){
        // try to resolve based on argv[0]
        if (argc>0){
            fs::path exe = fs::absolute(argv[0]);
            fs::path exeDir = exe.parent_path();
            fs::path parent = exeDir.parent_path();
            fs::path candidate = parent / "data"; // project_root/data when exe in build/
            if (fs::exists(candidate)) dataDir = candidate;
            else {
                // also try exeDir/data (in case user placed data next to exe)
                candidate = exeDir / "data";
                if (fs::exists(candidate)) dataDir = candidate;
            }
        }
    }
    std::string dir = dataDir.string();
    try {
        auto data = parseData(dir, false); // include campus edges; access filtering handled at runtime
        PathFinder pf(data);
        // Simple JSON API mode: --api --start S --end T --identity ID
        if (argc>1){
            std::string arg1 = argv[1];
            if (arg1=="--api"){
                std::string start="", end=""; int identity=0;
                for (int i=2;i<argc;i++){
                    std::string a = argv[i];
                    if (a=="--start" && i+1<argc) { start = argv[++i]; }
                    else if (a=="--end" && i+1<argc) { end = argv[++i]; }
                    else if (a=="--identity" && i+1<argc) { identity = std::stoi(argv[++i]); }
                }
                if (start.size()!=1 || end.size()!=1){ std::cerr<<"{\"status\":\"error\",\"message\":\"invalid start/end\"}"<<std::endl; return 1; }
                char s = start[0], t = end[0];
                if (!data.graph.hasNode(s) || !data.graph.hasNode(t)){ std::cerr<<"{\"status\":\"error\",\"message\":\"unknown node\"}"<<std::endl; return 1; }
                // produce JSON for requested identity
                std::ostringstream out;
                out<<"{";
                out<<"\"status\":\"ok\",";
                out<<"\"start\":\""<<s<<"\",";
                out<<"\"end\":\""<<t<<"\",";
                out<<"\"identity\":"<<identity<<",";
                // walk/bike/car K-shortest
                int K=3;
                auto walkPaths = pf.kShortestPaths(s,t,K,identity,false);
                out<<"\"routes\": [";
                bool firstR=true;
                for (auto &p: walkPaths){
                    if (!firstR) out<<","; firstR=false;
                    auto desc = pf.describePathWithRoads(p);
                    out<<"{";
                    out<<"\"path\": \"";
                    for (size_t i=0;i<p.size();++i){ out<<p[i]; if (i+1<p.size()) out<<"->"; }
                    out<<"\",";
                    out<<"\"nodes\": [";
                    for (size_t i=0;i<p.size();++i){ if (i) out<<","; out<<"\""<<p[i]<<"\""; }
                    out<<"],";
                    out<<"\"distance\":"<<std::fixed<<std::setprecision(2)<<desc.first<<",";
                    out<<"\"segments\": [";
                    for (size_t i=0;i<desc.second.size();++i){ if (i) out<<","; out<<"\""<<desc.second[i]<<"\""; }
                    out<<"],";
                    out<<"\"times\": {";
                    out<<"\"walk\":"<<pf.timeForPath(p,"walk")<<",";
                    out<<"\"bike\":"<<pf.timeForPath(p,"bike")<<",";
                    out<<"\"car\":"<<(pf.pathCarAllowed(p)? pf.timeForPath(p,"car") : -1)<<"}";
                    out<<"}";
                }
                out<<"],";
                // bus options
                auto busList = (s==t)? std::vector<Plan>() : pf.busPlans(s,t,identity);
                out<<"\"buses\": [";
                bool firstB=true;
                for (auto &bp: busList){ if (!firstB) out<<","; firstB=false;
                    out<<"{";
                    out<<"\"totalDistance\":"<<std::fixed<<std::setprecision(2)<<bp.totalDistance<<",";
                    out<<"\"totalTime\":"<<std::fixed<<std::setprecision(2)<<bp.totalTime<<",";
                    out<<"\"steps\": [";
                    bool firstStep=true;
                    for (auto &st: bp.steps){ if (!firstStep) out<<","; firstStep=false;
                        out<<"{";
                        out<<"\"mode\":\""<<st.mode<<"\",";
                        out<<"\"path\": [";
                        for (size_t i=0;i<st.path.size();++i){ if (i) out<<","; out<<"\""<<st.path[i]<<"\""; }
                        out<<"],";
                        out<<"\"distance\":"<<std::fixed<<std::setprecision(2)<<st.distance<<",";
                        out<<"\"time\":"<<std::fixed<<std::setprecision(2)<<st.time<<",";
                        out<<"\"note\":\""<<st.note<<"\"";
                        out<<"}";
                    }
                    out<<"]}";
                }
                out<<"]";
                out<<"}";
                std::cout<<out.str()<<std::endl;
                return 0;
            }
        }
        // interactive loop: repeatedly ask for start/end until user types "quit"
        while (true) {
            std::cout << "\nEnter start point (uppercase letter, e.g. A), or type \"quit\" to exit: ";
            char s,t; std::string tmp;
            if(!(std::cin>>tmp)) break; if (tmp == "quit" || tmp == "QUIT") break; s = tmp[0];
            std::cout << "Enter end point (uppercase letter, e.g. Z), or type \"quit\" to exit: ";
            if(!(std::cin>>tmp)) break; if (tmp == "quit" || tmp == "QUIT") break; t = tmp[0];
            if (!data.graph.hasNode(s) || !data.graph.hasNode(t)){
                std::cerr<<"Start or end point does not exist."<<std::endl; continue;
            }

            // identities to evaluate: visitor (no campus), FDU, SHUTCM, both
            std::vector<std::pair<int,std::string>> identities = {
                {0, "Visitor (no campus access)"},
                {1, "FDU member (can enter FDU)"},
                {2, "SHUTCM member (can enter SHUTCM)"},
                {3, "Both campuses access"}
            };

            // compute baseline for visitor (no campus) using K-shortest (K=3)
            int K = 3;
            auto walkPaths = pf.kShortestPaths(s,t,K,0,false);
            auto bikePaths = pf.kShortestPaths(s,t,K,0,false);
            auto carPaths = pf.kShortestPaths(s,t,K,0,true);
            auto baseBusList = (s==t)? std::vector<Plan>() : pf.busPlans(s,t,0);
            std::vector<std::string> printedKeys; // record printed walk-path keys to suppress duplicate identity outputs

            std::cout<<"\n===== Route summary (Visitor / no campus access) ====="<<std::endl;
            std::cout<<std::fixed<<std::setprecision(2);
            if (!walkPaths.empty()){
                // collect unique candidate paths among walk/bike/car (string key)
                std::vector<std::vector<char>> uniquePaths;
                auto pushUnique=[&](const std::vector<char>&p){ std::string key(p.begin(),p.end()); for (auto &q: uniquePaths) if (std::string(q.begin(),q.end())==key) return; uniquePaths.push_back(p); };
                for (auto &p: walkPaths) pushUnique(p);
                for (auto &p: bikePaths) pushUnique(p);
                for (auto &p: carPaths) pushUnique(p);
                // sort by distance (use describePathWithRoads)
                std::sort(uniquePaths.begin(), uniquePaths.end(), [&](const std::vector<char>&a, const std::vector<char>&b){
                    double da = pf.describePathWithRoads(a).first;
                    double db = pf.describePathWithRoads(b).first;
                    return da < db;
                });
                // print up to K (numbered) but filter out options much longer than the best (1.5x)
                int shown=0;
                int routeIdx=1;
                if (!uniquePaths.empty()){
                    double bestDist = pf.describePathWithRoads(uniquePaths.front()).first;
                    for (auto &path: uniquePaths){
                        auto desc = pf.describePathWithRoads(path);
                        if (desc.first > 3 * bestDist) continue; // skip clearly worse routes
                        if (shown++>=K) break;
                        std::cout<<"\nRoute "<<routeIdx++<<": ";
                        for (size_t i=0;i<path.size();++i){ std::cout<<path[i]; if (i+1<path.size()) std::cout<<"->"; }
                        std::cout<<std::endl;
                        std::cout<<"  Total distance: "<<std::fixed<<std::setprecision(2)<<desc.first<<" m"<<std::endl;
                        for (auto &seg: desc.second) std::cout<<"  - "<<seg<<std::endl;
                        // print times for each mode if applicable
                        double walkTime = pf.timeForPath(path, "walk");
                        double bikeTime = pf.timeForPath(path, "bike");
                        bool carAllowed = pf.pathCarAllowed(path);
                        double carTime = carAllowed? pf.timeForPath(path, "car") : -1;
                        std::cout<<"  Modes: Walk="<<std::fixed<<std::setprecision(2)<<walkTime<<" min, Bike="<<bikeTime<<" min, Car="<<(carTime<0? -1.0: carTime)<<" min"<<std::endl;
                    }
                }
            } else {
                std::cout<<"No walking route available for visitor."<<std::endl;
            }
            // bus baseline: show up to 3 best bus variants if they improve over walking
            if (!baseBusList.empty() && !walkPaths.empty()){
                auto bestBus = baseBusList.front(); for (auto &bp: baseBusList) if (bp.totalTime < bestBus.totalTime) bestBus = bp;
                double walkBest = pf.timeForPath(walkPaths.front(), "walk");
                if (bestBus.totalTime < walkBest){
                    // sort bus options by total time and show top 3
                    // group bus plans by the bus-ride node sequence so multiple bus numbers on same segment are merged
                    std::sort(baseBusList.begin(), baseBusList.end(), [](const Plan&a,const Plan&b){ return a.totalTime < b.totalTime; });
                    std::vector<Plan> grouped;
                    std::vector<std::string> groupNotes;
                    for (auto &bb: baseBusList){
                        // find bus step
                        std::string rideKey="";
                        std::string preKey=""; std::string postKey="";
                        std::string note = "";
                        for (auto &step: bb.steps){ if (step.mode=="bus"){ for (char c: step.path) rideKey.push_back(c); note = step.note; } }
                        // pre/post path keys for disambiguation
                        if (!bb.steps.empty()){ auto &s0 = bb.steps.front(); for (char c: s0.path) preKey.push_back(c); auto &s2 = bb.steps.back(); for (char c: s2.path) postKey.push_back(c); }
                        std::string fullKey = rideKey + "|" + preKey + "|" + postKey;
                        bool found=false;
                        for (size_t gi=0; gi<grouped.size(); ++gi){
                            // compare ride sequences
                            std::string existingRide=""; auto &g = grouped[gi];
                            for (auto &step: g.steps) if (step.mode=="bus") for (char c: step.path) existingRide.push_back(c);
                            if (existingRide == rideKey){
                                // merge note (append bus number if new)
                                if (groupNotes[gi].find(note)==std::string::npos) groupNotes[gi] += ", " + note;
                                // keep the fastest plan as representative
                                if (bb.totalTime < grouped[gi].totalTime) grouped[gi] = bb;
                                found=true; break;
                            }
                        }
                        if (!found){ grouped.push_back(bb); groupNotes.push_back(note); }
                    }
                    int bshown = 0;
                    for (size_t gi=0; gi<grouped.size() && bshown<3; ++gi){
                        auto &bb = grouped[gi];
                        std::string combinedNote = groupNotes[gi];
                        ++bshown;
                        std::cout<<"\nBus option "<<bshown<<": Distance="<<std::fixed<<std::setprecision(2)<<bb.totalDistance<<" m, Time="<<bb.totalTime<<" min"<<std::endl;
                        int stepIdx=1;
                        for (auto &step: bb.steps){
                            std::cout<<"  Step "<<stepIdx++<<": "<<step.mode<<" ("<<std::fixed<<std::setprecision(2)<<step.distance<<" m, "<<step.time<<" min)";
                            if (step.mode=="bus") std::cout<<" - "<<combinedNote;
                            else if (!step.note.empty()) std::cout<<" - "<<step.note;
                            std::cout<<"  Route: ";
                            for (size_t i=0;i<step.path.size();++i){ std::cout<<step.path[i]; if (i+1<step.path.size()) std::cout<<"->"; }
                            std::cout<<std::endl;
                            auto segs = pf.describePathWithRoads(step.path).second;
                            for (auto &seg: segs) std::cout<<"    - "<<seg<<std::endl;
                        }
                    }
                }
            }

            // prepare baseline walk path/time for comparison
            bool baselineWalkExists = !walkPaths.empty();
            std::vector<char> baselineWalkPath;
            double baselineWalkTime = 0.0;
            if (baselineWalkExists){ baselineWalkPath = walkPaths.front(); baselineWalkTime = pf.timeForPath(baselineWalkPath, "walk"); }

            // now check identities with campus access; only print if they provide improvement or different path
            for (auto &id : std::vector<std::pair<int,std::string>>{{1,"FDU member (can enter FDU)"},{2,"SHUTCM member (can enter SHUTCM)"},{3,"Both campuses access"}}){
                int flag = id.first; std::string idName = id.second;
                Plan w = pf.walkingPlan(s,t,flag);
                Plan b = pf.vehiclePlan(s,t,flag,"bike");
                Plan c = pf.vehiclePlan(s,t,flag,"car");
                auto busList = (s==t)? std::vector<Plan>() : pf.busPlans(s,t,flag);
                bool better = false;
                // consider better if walking path differs or time improved relative to baseline
                if (w.steps.empty() && baselineWalkExists) better = false;
                else if (!w.steps.empty() && (!baselineWalkExists || w.totalTime + 1e-6 < baselineWalkTime)) better = true;
                else {
                    // check path difference
                    if (!w.steps.empty() && baselineWalkExists){ if (w.steps[0].path != baselineWalkPath) better = true; }
                }
                if (!better) continue; // skip printing identical/no-improvement cases

                    // avoid duplicate identity outputs: if this walking path was already printed, skip
                    std::string thisKey;
                    if (!w.steps.empty()) thisKey = std::string(w.steps[0].path.begin(), w.steps[0].path.end());
                    bool already=false; for (auto &k: printedKeys) if (k==thisKey) { already=true; break; }
                    if (already) continue;

                std::cout<<"\n===== Identity: "<<idName<<" ====="<<std::endl;
                if (!w.steps.empty()){
                    auto path = w.steps[0].path;
                    std::cout<<"Route: "; for (size_t i=0;i<path.size();++i){ std::cout<<path[i]; if (i+1<path.size()) std::cout<<"->"; } std::cout<<std::endl;
                    auto desc = pf.describePathWithRoads(path);
                    std::cout<<"  Total distance: "<<std::fixed<<std::setprecision(2)<<desc.first<<" m"<<std::endl;
                    for (auto &seg: desc.second) std::cout<<"  - "<<seg<<std::endl;
                    // decide bike/car availability: car only shown if every edge allows car
                    double bikeTime = (b.steps.empty()? -1.0 : b.totalTime);
                    double carTime = -1.0;
                    if (pf.pathCarAllowed(path)) carTime = (c.steps.empty()? -1.0 : c.totalTime);
                    std::cout<<"  Modes: Walk="<<std::fixed<<std::setprecision(2)<<w.totalTime<<" min, Bike="<<bikeTime<<" min, Car="<<(carTime<0? -1.0: carTime)<<" min"<<std::endl;
                    // record printed identity path to avoid duplicate prints for other identities
                    printedKeys.push_back(thisKey);
                } else {
                    std::cout<<"No walking route available for this identity."<<std::endl;
                }
                if (!busList.empty()){
                    auto bestBus = busList.front(); for (auto &bp: busList) if (bp.totalTime < bestBus.totalTime) bestBus = bp;
                        if (!w.steps.empty() && bestBus.totalTime < w.totalTime){
                            std::cout<<"\nBus (best): Distance="<<bestBus.totalDistance<<" m, Time="<<std::fixed<<std::setprecision(2)<<bestBus.totalTime<<" min"<<std::endl;
                            int stepIdx=1;
                            for (auto &step: bestBus.steps){
                                if (step.mode=="walk" || step.mode=="bike"){
                                    std::cout<<"  Step "<<stepIdx++<<": "<<step.mode<<" ("<<std::fixed<<std::setprecision(2)<<step.distance<<" m, "<<step.time<<" min) - "<<step.note<<"  Route: ";
                                    for (size_t i=0;i<step.path.size();++i){ std::cout<<step.path[i]; if (i+1<step.path.size()) std::cout<<"->"; }
                                    std::cout<<std::endl;
                                    auto segs = pf.describePathWithRoads(step.path).second;
                                    for (auto &seg: segs) std::cout<<"    - "<<seg<<std::endl;
                                } else if (step.mode=="bus"){
                                    char board = step.path.front(); char alight = step.path.back();
                                    std::string busNo = step.note;
                                    std::cout<<"  Step "<<stepIdx++<<": bus ("<<std::fixed<<std::setprecision(2)<<step.distance<<" m, "<<step.time<<" min) - "<<busNo<<" from "<<data.busStopNames[board]<<" to "<<data.busStopNames[alight]<<"  Route: ";
                                    for (size_t i=0;i<step.path.size();++i){ std::cout<<step.path[i]; if (i+1<step.path.size()) std::cout<<"->"; }
                                    std::cout<<std::endl;
                                    auto segs = pf.describePathWithRoads(step.path).second;
                                    for (auto &seg: segs) std::cout<<"    - "<<seg<<std::endl;
                                }
                            }
                        }
                }
            }
            std::cout<<"\n====================================================\n"<<std::endl;
            // loop again
        }
    } catch (const std::exception &e){
        std::cerr<<"Error: "<<e.what()<<std::endl; return 1;
    }
    return 0;
}
