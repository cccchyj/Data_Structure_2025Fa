#include "PathFinder.h"
#include <iostream>
#include <queue>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include <unordered_map>

PathFinder::PathFinder(const DataBundle &d): data(d) {}

void PathFinder::dijkstra(int src, std::vector<double> &dist, std::vector<int> &prev, 
                          int identityFlag, bool carMode) const {
    // wrapper: no banned edges/nodes
    std::unordered_set<unsigned long long> bannedE;
    std::unordered_set<int> bannedN;
    dijkstra(src, dist, prev, identityFlag, carMode, bannedE, bannedN);
}

// Helper to combine u,v into a 64-bit key
static unsigned long long edgeKey(int u, int v){
    return ( (unsigned long long)u << 32 ) | (unsigned long long)(unsigned int)v;
}

void PathFinder::dijkstra(int src, std::vector<double> &dist, std::vector<int> &prev, 
                          int identityFlag, bool carMode, 
                          const std::unordered_set<unsigned long long> &bannedEdges, 
                          const std::unordered_set<int> &bannedNodes) const {
    // Standard Dijkstra's algorithm with access checks.
    // identityFlag: bit0 = FDU allowed, bit1 = SHUTCM allowed.
    // carMode=true prevents traversing campus-only edges even if identity permits.

    int n = data.graph.nodeCount();
    using P = std::pair<double,int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    // Standard initialization: everything is unreachable except the source.
    dist.assign(n, std::numeric_limits<double>::infinity());
    prev.assign(n, -1);
    dist[src]=0;
    pq.push({0,src});

    while(!pq.empty()){

        auto [d,u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;

        // banned node check
        if (bannedNodes.find(u) != bannedNodes.end()) continue;

        // iterate neighbors
        for (auto &e: data.graph.neighbors(u)){

            // banned edge check
            if (bannedEdges.find(edgeKey(u,e.to)) != bannedEdges.end()) continue;

            // access permissions: 0 public, 1 FDU-only, 2 SHUTCM-only
            if (e.access == 1 && !(identityFlag & 1)) continue;
            if (e.access == 2 && !(identityFlag & 2)) continue;

            // car restriction: campus edges forbidden
            if (carMode && e.access != 0) continue;

            int v = e.to;
            double nd = d + e.dist;

            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({nd, v});
            }
        }
    }
}


// Yen's K-shortest paths algorithm (simple implementation)
std::vector<std::vector<char>> PathFinder::kShortestPaths(char s, char t, int K, 
                        int identityFlag, bool carMode) const {
    std::vector<std::vector<char>> A;                   // shortest paths
    std::vector<std::pair<double,std::vector<char>>> B; // candidate spur paths (dist, path)

    if (!data.graph.hasNode(s) || !data.graph.hasNode(t)) return A;
    int si = data.graph.nodeIndex(s), ti = data.graph.nodeIndex(t);
    
    // first shortest path
    std::vector<double> dist; std::vector<int> prev;
    dijkstra(si, dist, prev, identityFlag, carMode);
    if (dist[ti] == std::numeric_limits<double>::infinity()) return A;
    auto p0 = reconstructPath(prev, ti);
    A.push_back(p0);

    for (int k = 1; k < K; ++k){ // K shortest paths except first
        // for each node in previous shortest path except last
        for (size_t i = 0; i + 1 < A[k-1].size(); ++i){
            char spurNode = A[k-1][i];
            // root path is nodes from 0..i
            std::vector<char> rootPath(A[k-1].begin(), A[k-1].begin()+i+1);
            // banned edges and nodes
            std::unordered_set<unsigned long long> bannedEdges;
            std::unordered_set<int> bannedNodes;

            // remove the edges that would create previously found paths with same root
            for (auto &p: A){
                if (p.size() > i && std::equal(rootPath.begin(), rootPath.end(), p.begin())){
                    int u = data.graph.nodeIndex(p[i]);
                    int v = data.graph.nodeIndex(p[i+1]);
                    bannedEdges.insert(edgeKey(u,v));
                }
            }
            // ban nodes in root path except spur node
            for (size_t j = 0; j < rootPath.size() - 1; ++j) bannedNodes.insert(data.graph.nodeIndex(rootPath[j]));

            // compute spur path from spurNode to target
            int spurIdx = data.graph.nodeIndex(spurNode);
            std::vector<double> sdist; std::vector<int> sprev;
            dijkstra(spurIdx, sdist, sprev, identityFlag, carMode, bannedEdges, bannedNodes);
            if (sdist[ti] == std::numeric_limits<double>::infinity()) continue;
            auto spurPart = reconstructPath(sprev, ti);

            // combine root + spurPart (avoid double counting spur node)
            std::vector<char> total = rootPath;
            total.insert(total.end(), spurPart.begin()+1, spurPart.end());

            // compute total distance
            double td = 0; 
            for (size_t z = 0; z + 1 < total.size(); ++z){ 
                td += data.graph.edgeDistance(data.graph.nodeIndex(total[z]), data.graph.nodeIndex(total[z+1])); 
            }
            B.emplace_back(td, total);
        }
        if (B.empty()) break;
        // choose shortest candidate from B
        std::sort(B.begin(), B.end(), [](auto &a, auto &b){ return a.first < b.first; });
        A.push_back(B.front().second);
        B.erase(B.begin());
    }
    return A;
}

std::vector<char> PathFinder::reconstructPath(const std::vector<int> &prev, int dst) const {
    std::vector<char> path;
    int cur = dst;
    if (cur<0) return path;
    while (cur!=-1){ path.push_back(data.graph.indexLabel(cur)); cur = prev[cur]; }
    std::reverse(path.begin(), path.end());
    return path;
}

Plan PathFinder::walkingPlan(char s, char t, int identityFlag) const {
    if (!data.graph.hasNode(s) || !data.graph.hasNode(t)) throw std::runtime_error("Unknown node");
    int si = data.graph.nodeIndex(s), ti = data.graph.nodeIndex(t);
    std::vector<double> dist; std::vector<int> prev;
    // walking uses campus edges if identity allows
    dijkstra(si, dist, prev, identityFlag, /*carMode=*/false);
    Plan p; p.totalDistance = 0; p.totalTime = 0;
    if (dist[ti]==std::numeric_limits<double>::infinity()) return p;
    auto path = reconstructPath(prev, ti);
    double totalDist = dist[ti];
    PlanStep step; step.mode="walk"; step.path=path; step.distance=totalDist; step.time=totalDist/sp.walk; step.note="";
    p.steps.push_back(step); p.totalDistance = totalDist; p.totalTime = step.time;
    return p;
}

Plan PathFinder::vehiclePlanFromPath(const std::vector<char> &path, const std::string &mode) const {
    Plan p; p.totalDistance=0; p.totalTime=0;
    if (path.empty()) return p;
    // sum distances along path
    double sum=0;
    for (size_t i=0;i+1<path.size();++i){
        int u = data.graph.nodeIndex(path[i]);
        int v = data.graph.nodeIndex(path[i+1]);
        double d = data.graph.edgeDistance(u,v);
        if (d<0) { sum = -1; break; }
        sum += d;
    }
    if (sum < 0) return p;
    double speed = (mode=="bike")?sp.bike:sp.car;
    PlanStep step; step.mode = mode; step.path = path; step.distance = sum; step.time = sum / speed; step.note="";
    p.steps.push_back(step); p.totalDistance = sum; p.totalTime = step.time;
    return p;
}

Plan PathFinder::vehiclePlan(char s, char t, int identityFlag, const std::string &mode) const {
    // Try to reuse walking path if possible (bike and car should follow same path generally)
    Plan walk = walkingPlan(s,t,identityFlag);
    if (walk.steps.empty()) return Plan();
    auto basePath = walk.steps[0].path;
    // check if car is allowed on basePath; if mode=="car" and any edge on basePath disallows car, compute car-specific shortest path
    if (mode=="car"){
        bool carOK = true;
        for (size_t i=0;i+1<basePath.size();++i){
            int u = data.graph.nodeIndex(basePath[i]);
            int v = data.graph.nodeIndex(basePath[i+1]);
            double d = data.graph.edgeDistance(u,v);
            // find edge in adjacency to get access
            bool found=false; int access=0;
            for (auto &e: data.graph.neighbors(u)) if (e.to==v){ found=true; access = e.access; break; }
            if (!found) { carOK=false; break; }
            if (access != 0) { carOK=false; break; }
        }
        if (carOK) return vehiclePlanFromPath(basePath, "car");
        // else compute a best car-then-walk plan: drive to nearest public node then walk into campus if needed
        return carThenWalkToCampus(s,t,identityFlag);
    } else {
        // bike: use base walking path but compute time with bike speed
        return vehiclePlanFromPath(basePath, "bike");
    }
}

Plan PathFinder::carThenWalkToCampus(char s, char t, int identityFlag) const {
    Plan out;
    if (!data.graph.hasNode(s) || !data.graph.hasNode(t)) return out;
    int si = data.graph.nodeIndex(s), ti = data.graph.nodeIndex(t);
    // run dijkstra for car from source (carMode=true, identityFlag irrelevant because cars cannot enter campus edges)
    std::vector<double> distCar; std::vector<int> prevCar;
    dijkstra(si, distCar, prevCar, /*identityFlag*/0, /*carMode*/true);
    // run dijkstra from target for walking with identityFlag to get distances from nodes to target
    std::vector<double> distToT; std::vector<int> prevToT;
    dijkstra(ti, distToT, prevToT, identityFlag, /*carMode*/false);
    double bestTime = std::numeric_limits<double>::infinity();
    int bestNode = -1;
    for (int v=0; v<data.graph.nodeCount(); ++v){
        if (distCar[v]==std::numeric_limits<double>::infinity()) continue;
        if (distToT[v]==std::numeric_limits<double>::infinity()) continue;
        double time = distCar[v]/sp.car + distToT[v]/sp.walk;
        if (time < bestTime){ bestTime = time; bestNode = v; }
    }
    if (bestNode==-1) return out;
    // reconstruct car path from source to bestNode
    auto carPath = reconstructPath(prevCar, bestNode);
    // reconstruct walk path from bestNode to target using prevToT: reconstructPath(prevToT, bestNode) returns path from target to bestNode, reverse it
    auto rev = reconstructPath(prevToT, bestNode);
    std::reverse(rev.begin(), rev.end());
    auto walkPath = rev;
    // build Plan
    PlanStep sc; sc.mode = "car"; sc.path = carPath; sc.distance = pathDistance(carPath); sc.time = sc.distance / sp.car; sc.note = "drive to campus entry";
    PlanStep sw; sw.mode = "walk"; sw.path = walkPath; sw.distance = pathDistance(walkPath); sw.time = sw.distance / sp.walk; sw.note = "walk into campus";
    out.steps.push_back(sc); out.steps.push_back(sw);
    out.totalDistance = sc.distance + sw.distance; out.totalTime = sc.time + sw.time;
    return out;
}

static double distanceAlongSequence(const Graph &g, const std::vector<char> &seq, int i, int j){
    double sum=0;
    if (i==j) return 0;
    int step = (i<j)?1:-1;
    for (int k=i; k!=j; k+=step){
        int u = g.nodeIndex(seq[k]);
        int v = g.nodeIndex(seq[k+step]);
        double d = g.edgeDistance(u,v);
        if (d<0) return -1.0;
        sum += d;
    }
    return sum;
}

double PathFinder::distanceBetweenNodes(char a, char b, int identityFlag) const {
    if (!data.graph.hasNode(a) || !data.graph.hasNode(b)) {
        return std::numeric_limits<double>::infinity();
    }
    int ai = data.graph.nodeIndex(a);
    int bi = data.graph.nodeIndex(b);
    std::vector<double> dist;
    std::vector<int> prev;
    dijkstra(ai, dist, prev, identityFlag, /*carMode*/false);
    return dist[bi];
}

std::vector<Plan> PathFinder::busPlans(char s, char t, int identityFlag) const {
    std::vector<Plan> out;
    if (!data.graph.hasNode(s) || !data.graph.hasNode(t)) return out;
    int si = data.graph.nodeIndex(s), ti = data.graph.nodeIndex(t);
    // precompute walking distances from source and to target using allowed walking edges
    std::vector<double> distFromS, distFromT; std::vector<int> prevS, prevT;
    dijkstra(si, distFromS, prevS, identityFlag, /*carMode*/false);
    dijkstra(ti, distFromT, prevT, identityFlag, /*carMode*/false);
    
    // Precompute walking distances between all bus stops
    std::unordered_map<std::string, double> busStopDistances;
    for (char stopA : data.busStops) {
        for (char stopB : data.busStops) {
            if (stopA == stopB) continue;
            std::string key = std::string(1, stopA) + ":" + std::string(1, stopB);
            busStopDistances[key] = distanceBetweenNodes(stopA, stopB, identityFlag);
        }
    }

    // Debug info
    std::cerr << "busPlans: s=" << s << " t=" << t << " identity=" << identityFlag << std::endl;
    std::cerr << "busStops size: " << data.busStops.size() << " busRoutes size: " << data.busRoutes.size() << std::endl;
    
    // Generate single bus plans
    for (const auto &br : data.busRoutes){
        std::cerr << "Processing bus route " << br.busNo << ", seq: ";
        for (char c : br.seq) std::cerr << c << " ";
        std::cerr << std::endl;
        
        for (size_t i=0;i<br.seq.size();++i){
            for (size_t j=0;j<br.seq.size();++j){
                if (i == j) continue;
                
                char stopA = br.seq[i];
                char stopB = br.seq[j];
                
                // only consider if stopA and stopB are actual bus stops
                if (std::find(data.busStops.begin(), data.busStops.end(), stopA) == data.busStops.end()) {
                    continue;
                }
                if (std::find(data.busStops.begin(), data.busStops.end(), stopB) == data.busStops.end()) {
                    continue;
                }
                
                int ai = data.graph.nodeIndex(stopA);
                int bi = data.graph.nodeIndex(stopB);
                
                // Skip candidates where start cannot walk to boarding stop or alighting stop cannot walk to destination
                if (distFromS[ai]==std::numeric_limits<double>::infinity()) {
                    continue;
                }
                if (distFromT[bi]==std::numeric_limits<double>::infinity()) {
                    continue;
                }
                
                double walk1 = distFromS[ai];
                double walk2 = distFromT[bi];
                
                double busdist = distanceAlongSequence(data.graph, br.seq, (int)i, (int)j);
                if (busdist < 0) {
                    continue;
                }
                
                // Calculate the direct walking distance from stopA to stopB
                double directWalkDist = distanceBetweenNodes(stopA, stopB, identityFlag);
                
                // Skip if the bus route is significantly longer than direct walking
                double busOverheadFactor = 1.5;
                if (busdist > directWalkDist * busOverheadFactor) {
                    continue;
                }
                
                Plan plan; 
                plan.totalDistance = walk1 + busdist + walk2; 
                plan.totalTime = 0;
                
                // walking to stop (only include if non-zero)
                PlanStep s1;
                s1.mode="walk";
                s1.distance = walk1;
                s1.time = (walk1>0)? walk1 / sp.walk : 0;
                s1.note="to bus stop";
                s1.path = reconstructPath(prevS, ai);

                // bus step
                PlanStep sb;
                sb.mode = "bus";
                sb.path.clear();
                sb.distance = busdist;
                sb.time = busdist / sp.bus;

                // handle both forward and reverse directions correctly
                if (i < j) {
                    for (size_t k=i; k<=j; ++k) sb.path.push_back(br.seq[k]);
                    std::string stopAName = data.busStopNames.count(stopA)? data.busStopNames.at(stopA) : std::string(1, stopA);
                    std::string stopBName = data.busStopNames.count(stopB)? data.busStopNames.at(stopB) : std::string(1, stopB);
                    sb.note = std::string("bus ") + br.busNo + std::string(" from ") + stopAName + std::string(" to ") + stopBName;
                } else {
                    for (int k = (int)i; k >= (int)j; --k) sb.path.push_back(br.seq[k]);
                    std::string stopAName = data.busStopNames.count(stopA)? data.busStopNames.at(stopA) : std::string(1, stopA);
                    std::string stopBName = data.busStopNames.count(stopB)? data.busStopNames.at(stopB) : std::string(1, stopB);
                    sb.note = std::string("bus ") + br.busNo + std::string(" from ") + stopAName + std::string(" to ") + stopBName;
                }

                // walking from stopB to dest (only include if non-zero)
                PlanStep s2;
                s2.mode="walk";
                s2.distance = walk2;
                s2.time = (walk2>0)? walk2 / sp.walk : 0;
                s2.note="from bus stop";
                s2.path = reconstructPath(prevT, bi);
                std::reverse(s2.path.begin(), s2.path.end());

                plan.steps.clear();
                if (s1.distance > 0) plan.steps.push_back(s1);
                plan.steps.push_back(sb);
                if (s2.distance > 0) plan.steps.push_back(s2);
                plan.totalTime = 0;
                for (auto &pst: plan.steps) plan.totalTime += pst.time;
                
                // collect raw candidate
                out.push_back(plan);
            }
        }
    }
    
    // Generate multi-bus plans (2 buses only for now, to keep complexity manageable)
    std::cerr << "Starting multi-bus plan generation..." << std::endl;
    std::cerr << "Number of bus routes: " << data.busRoutes.size() << std::endl;
    std::cerr << "Number of bus stops: " << data.busStops.size() << std::endl;
    
    int totalTransferPoints = 0;
    for (const auto &br1 : data.busRoutes) {
        for (const auto &br2 : data.busRoutes) {
            if (br1.busNo == br2.busNo) continue; // same bus, no need to transfer
            
            // Find all possible transfer points between bus1 and bus2
            for (char transferStop : data.busStops) {
                // Check if transferStop is in both bus routes
                bool inBus1 = std::find(br1.seq.begin(), br1.seq.end(), transferStop) != br1.seq.end();
                bool inBus2 = std::find(br2.seq.begin(), br2.seq.end(), transferStop) != br2.seq.end();
                if (inBus1 && inBus2) {
                    totalTransferPoints++;
                    std::cerr << "Found transfer point " << transferStop << " between bus " << br1.busNo << " and bus " << br2.busNo << std::endl;
                }
                if (!inBus1 || !inBus2) continue;
                
                // For bus1, find all possible start stops that can reach transferStop
                for (size_t i1=0; i1<br1.seq.size(); ++i1) {
                    char stopA1 = br1.seq[i1];
                    if (std::find(data.busStops.begin(), data.busStops.end(), stopA1) == data.busStops.end()) {
                        continue;
                    }
                    
                    int ai1 = data.graph.nodeIndex(stopA1);
                    if (distFromS[ai1] == std::numeric_limits<double>::infinity()) {
                        continue;
                    }
                    
                    // Find the index of transferStop in bus1's sequence
                    size_t transferIdx1 = std::find(br1.seq.begin(), br1.seq.end(), transferStop) - br1.seq.begin();
                    if (i1 == transferIdx1) continue; // no need to ride bus if already at transfer stop
                    
                    double bus1dist = distanceAlongSequence(data.graph, br1.seq, (int)i1, (int)transferIdx1);
                    if (bus1dist < 0) continue;
                    
                    // For bus2, find all possible end stops that can be reached from transferStop
                    for (size_t i2=0; i2<br2.seq.size(); ++i2) {
                        char stopB2 = br2.seq[i2];
                        if (std::find(data.busStops.begin(), data.busStops.end(), stopB2) == data.busStops.end()) {
                            continue;
                        }
                        
                        int bi2 = data.graph.nodeIndex(stopB2);
                        if (distFromT[bi2] == std::numeric_limits<double>::infinity()) {
                            continue;
                        }
                        
                        // Find the index of transferStop in bus2's sequence
                        size_t transferIdx2 = std::find(br2.seq.begin(), br2.seq.end(), transferStop) - br2.seq.begin();
                        if (i2 == transferIdx2) continue; // no need to ride bus if already at transfer stop
                        
                        double bus2dist = distanceAlongSequence(data.graph, br2.seq, (int)transferIdx2, (int)i2);
                        if (bus2dist < 0) continue;
                        
                        // Calculate the total distance and time
                        double walk1 = distFromS[ai1];
                        double walk2 = distFromT[bi2];
                        double totalDistance = walk1 + bus1dist + bus2dist + walk2;
                        
                        // Calculate the direct walking distance from start to end for comparison
                        double directWalkDist = distanceBetweenNodes(s, t, identityFlag);
                        
                        // Allow multi-bus routes if they're not too much longer than direct walking
                        // Using a higher threshold for multi-bus routes since they might involve more distance but be faster
                        double multiBusOverheadFactor = 4.0;
                        if (totalDistance > directWalkDist * multiBusOverheadFactor) {
                            continue;
                        }
                        
                        // Build the plan
                        Plan plan;
                        plan.totalDistance = totalDistance;
                        plan.totalTime = 0;
                        
                        // Step 1: Walk from start to bus1 stop (only include if non-zero)
                        PlanStep s1;
                        s1.mode = "walk";
                        s1.distance = walk1;
                        s1.time = (walk1 > 0) ? walk1 / sp.walk : 0;
                        s1.note = "to bus stop";
                        s1.path = reconstructPath(prevS, ai1);
                        if (s1.distance > 0) plan.steps.push_back(s1);
                        
                        // Step 2: Bus1 ride from stopA1 to transferStop
                        PlanStep b1;
                        b1.mode = "bus";
                        b1.distance = bus1dist;
                        b1.time = bus1dist / sp.bus;
                        
                        if (i1 < transferIdx1) {
                            for (int k = (int)i1; k <= (int)transferIdx1; ++k) {
                                b1.path.push_back(br1.seq[k]);
                            }
                        } else {
                            for (int k = (int)i1; k >= (int)transferIdx1; --k) {
                                b1.path.push_back(br1.seq[k]);
                            }
                        }
                        
                        std::string stopA1Name = data.busStopNames.count(stopA1) ? data.busStopNames.at(stopA1) : std::string(1, stopA1);
                        std::string transferStopName = data.busStopNames.count(transferStop) ? data.busStopNames.at(transferStop) : std::string(1, transferStop);
                        b1.note = std::string("bus ") + br1.busNo + std::string(" from ") + stopA1Name + std::string(" to ") + transferStopName;
                        plan.steps.push_back(b1);
                        
                        // Step 3: Transfer walk (if zero-distance transfer, merge bus1 and bus2 into a single bus segment)
                        bool zeroTransfer = true; // transfer step modeled as zero distance in this simplified model
                        std::string stopB2Name = data.busStopNames.count(stopB2) ? data.busStopNames.at(stopB2) : std::string(1, stopB2);
                            if (zeroTransfer) {
                                // merge b1 and b2 into a single bus step but avoid duplicating the first-bus prefix
                                PlanStep mergedBus;
                                mergedBus.mode = "bus";
                                mergedBus.distance = bus1dist + bus2dist;
                                mergedBus.time = mergedBus.distance / sp.bus;
                                // merged path should start from the transfer stop and include only the second-bus segment
                                mergedBus.path.push_back(transferStop);
                                if (transferIdx2 < i2) {
                                    for (int k = (int)transferIdx2+1; k <= (int)i2; ++k) mergedBus.path.push_back(br2.seq[k]);
                                } else {
                                    for (int k = (int)transferIdx2-1; k >= (int)i2; --k) mergedBus.path.push_back(br2.seq[k]);
                                }

                                // merged step should describe the continuation on the second bus only
                                std::string busNoteB = std::string("bus ") + br2.busNo + std::string(" from ") + transferStopName + std::string(" to ") + stopB2Name;
                                mergedBus.note = busNoteB;

                                plan.steps.push_back(mergedBus);
                            } else {
                            // Step 3: Transfer walk
                            PlanStep transferStep;
                            transferStep.mode = "walk";
                            transferStep.distance = 0;
                            transferStep.time = 0; // no time for transfer in this simplified model
                            transferStep.note = "transfer at " + transferStopName;
                            transferStep.path = {transferStop}; // just the transfer stop
                            plan.steps.push_back(transferStep);

                            // Step 4: Bus2 ride from transferStop to stopB2
                            PlanStep b2;
                            b2.mode = "bus";
                            b2.distance = bus2dist;
                            b2.time = bus2dist / sp.bus;
                            if (transferIdx2 < i2) {
                                for (int k = (int)transferIdx2; k <= (int)i2; ++k) {
                                    b2.path.push_back(br2.seq[k]);
                                }
                            } else {
                                for (int k = (int)transferIdx2; k >= (int)i2; --k) {
                                    b2.path.push_back(br2.seq[k]);
                                }
                            }
                            std::string stopB2Name = data.busStopNames.count(stopB2) ? data.busStopNames.at(stopB2) : std::string(1, stopB2);
                            b2.note = std::string("bus ") + br2.busNo + std::string(" from ") + transferStopName + std::string(" to ") + stopB2Name;
                            plan.steps.push_back(b2);
                        }
                        
                        // Step 5: Walk from bus2 stop to end (only include if non-zero)
                        PlanStep s2;
                        s2.mode = "walk";
                        s2.distance = walk2;
                        s2.time = (walk2 > 0) ? walk2 / sp.walk : 0;
                        s2.note = "from bus stop";
                        s2.path = reconstructPath(prevT, bi2);
                        std::reverse(s2.path.begin(), s2.path.end());
                        if (s2.distance > 0) plan.steps.push_back(s2);
                        
                        // Calculate total time
                        for (const auto &step : plan.steps) {
                            plan.totalTime += step.time;
                        }
                        
                        // Add the multi-bus plan to the list
                        out.push_back(plan);
                    }
                }
            }
        }
    }
    
    std::cerr << "Total transfer points found: " << totalTransferPoints << std::endl;
    std::cerr << "Total plans before merging: " << out.size() << std::endl;
    
    // Merge plans that share the same pre-path, ride-path and post-path but differ only by busNo
    // Build a map key -> index
    std::unordered_map<std::string, int> idx;
    std::vector<Plan> merged;
    for (auto &p : out){
        // extract keys
        std::string preKey, rideKey, postKey;
        if (!p.steps.empty()){
            auto &pre = p.steps.front(); for (char c: pre.path) preKey.push_back(c);
            // find bus step(s)
            std::string combinedRideKey;
            for (auto &st: p.steps) {
                if (st.mode == "bus") {
                    for (char c: st.path) combinedRideKey.push_back(c);
                    combinedRideKey.push_back('|'); // separator between bus segments
                }
            }
            rideKey = combinedRideKey;
            if (p.steps.size()>=2){
                auto &post = p.steps.back(); for (char c: post.path) postKey.push_back(c);
            }
        }
        std::string key = preKey + "|" + rideKey + "|" + postKey;
        auto it = idx.find(key);
        if (it == idx.end()){
            int id = (int)merged.size();
            idx[key] = id;
            merged.push_back(p);
        } else {
            // merge bus numbers into existing plan's bus step notes
            int id = it->second;
            
            // Process each bus step in both plans
            auto &existingPlan = merged[id];
            for (size_t stepIdx = 0; stepIdx < p.steps.size(); ++stepIdx) {
                auto &newStep = p.steps[stepIdx];
                if (newStep.mode != "bus") continue;
                
                auto &existingStep = existingPlan.steps[stepIdx];
                if (existingStep.mode != "bus") continue;
                
                // extract busNos from notes
                auto extractBusNo=[&](const std::string &note)->std::string{
                    size_t pos = note.find(" ");
                    if (pos==std::string::npos) return note;
                    size_t pos2 = note.find(" ", pos+1);
                    if (pos2==std::string::npos) pos2 = note.size();
                    return note.substr(pos+1, pos2-pos-1);
                };
                
                std::string nb = extractBusNo(newStep.note);
                if (existingStep.note.find(nb) == std::string::npos){
                    // append " / nb" after the bus number part
                    // replace "bus <old>" with "bus <old> / <nb>"
                    size_t p1 = existingStep.note.find(" "); 
                    size_t p2 = existingStep.note.find(" ", p1+1);
                    if (p1!=std::string::npos){
                        std::string head = existingStep.note.substr(0, p2);
                        std::string tail = existingStep.note.substr(p2);
                        existingStep.note = head + std::string(" / ") + nb + tail;
                    } else {
                        existingStep.note += std::string(" / ") + nb;
                    }
                }
            }
            
            // choose minimal totalTime representative
            if (p.totalTime < merged[id].totalTime) merged[id] = p;
        }
    }
    
    // Build final out list: for each merged plan, also add a bike-variant (convert walk -> bike)
    std::vector<Plan> finalOut;
    for (auto &mp : merged){
        // annotate walk steps with bike times so the UI/CLI can show both options in one plan
        for (auto &st: mp.steps){
            if (st.mode == "walk"){
                double bikeTime = (st.distance>0)? (st.distance / sp.bike) : 0.0;
                // append bike time to note (avoid duplicate)
                std::ostringstream ss; ss<<std::fixed<<std::setprecision(2)<<bikeTime;
                std::string bikeNote = std::string(" (bike: ") + ss.str() + std::string(" min)");
                if (st.note.find("(bike:") == std::string::npos) st.note += bikeNote;
            }
        }
        finalOut.push_back(mp);
    }
    
    // Sort plans by total time in ascending order
    std::sort(finalOut.begin(), finalOut.end(), [](const Plan &a, const Plan &b) {
        return a.totalTime < b.totalTime;
    });
    
    std::cerr << "Total plans after merging: " << merged.size() << std::endl;
    std::cerr << "Total plans after adding bike variants: " << finalOut.size() << std::endl;
    
    return finalOut;
}

double PathFinder::pathDistance(const std::vector<char> &path) const {
    double sum=0;
    for (size_t i=0; i+1<path.size(); ++i) {
        int u = data.graph.nodeIndex(path[i]);
        int v = data.graph.nodeIndex(path[i+1]);
        double d = data.graph.edgeDistance(u, v);
        if (d<0) return -1;
        sum += d;
    }
    return sum;
}

std::pair<double, std::vector<std::string>> PathFinder::describePathWithRoads(const std::vector<char> &path) const {
    std::pair<double, std::vector<std::string>> out;
    out.first = 0.0;
    if (path.size() < 2) return out;
    // helper to get edge info between two nodes
    auto getEdgeInfo = [&](char a, char b)->std::pair<double,std::string>{
        int u = data.graph.nodeIndex(a);
        int v = data.graph.nodeIndex(b);
        for (auto &e: data.graph.neighbors(u)){
            if (e.to == v) return {e.dist, e.roadName};
        }
        return {-1.0, std::string("")};
    };

    std::string curRoad;
    std::vector<char> curSeq;
    for (size_t i=0;i+1<path.size();++i){
        char a = path[i]; char b = path[i+1];
        auto info = getEdgeInfo(a,b);
        double d = info.first; std::string rn = info.second;
        if (d < 0) return out; // unknown edge
        out.first += d;
        if (i==0){ curRoad = rn; curSeq.clear(); curSeq.push_back(a); curSeq.push_back(b); }
        else {
            if (rn == curRoad){ curSeq.push_back(b); }
            else {
                // flush curSeq with curRoad
                std::string s = curRoad + ": ";
                for (size_t k=0;k<curSeq.size();++k){ s.push_back(curSeq[k]); if (k+1<curSeq.size()) s += "->"; }
                out.second.push_back(s);
                // start new
                curRoad = rn; curSeq.clear(); curSeq.push_back(a); curSeq.push_back(b);
            }
        }
    }
    if (!curSeq.empty()){
        std::string s = curRoad + ": ";
        for (size_t k=0;k<curSeq.size();++k){ s.push_back(curSeq[k]); if (k+1<curSeq.size()) s += "->"; }
        out.second.push_back(s);
    }
    return out;
}

double PathFinder::timeForPath(const std::vector<char> &path, const std::string &mode) const {
    double d = pathDistance(path);
    if (d < 0) return -1.0;
    if (mode == "walk") return d / sp.walk;
    if (mode == "bike") return d / sp.bike;
    if (mode == "car") return d / sp.car;
    if (mode == "bus") return d / sp.bus;
    return -1.0;
}

bool PathFinder::pathCarAllowed(const std::vector<char> &path) const {
    if (path.size() < 2) return true;
    for (size_t i=0;i+1<path.size();++i){
        int u = data.graph.nodeIndex(path[i]);
        int v = data.graph.nodeIndex(path[i+1]);
        bool found=false; int access=0;
        for (auto &e: data.graph.neighbors(u)){
            if (e.to == v){ found=true; access = e.access; break; }
        }
        if (!found) return false;
        if (access != 0) return false; // non-public edge
    }
    return true;
}