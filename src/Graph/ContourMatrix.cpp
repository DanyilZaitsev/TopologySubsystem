#include "ContourMatrix.h"
#include <stack>
#include <queue>
#include <algorithm>
#include <iostream>
#include <unordered_set>

ContourMatrix::ContourMatrix(const Graph& g, const Decomposition& d)
    : graph(g), dec(d)
{
    // find edge id range from original edges
    const auto& edges = graph.getEdges();
    if (!edges.empty()) {
        minEdgeId = edges.front().id;
        maxEdgeId = edges.front().id;
        for (const auto &e : edges) {
            minEdgeId = std::min(minEdgeId, e.id);
            maxEdgeId = std::max(maxEdgeId, e.id);
        }
    } else {
        minEdgeId = 0; maxEdgeId = -1;
    }

    int V = graph.numVertices();
    treeAdj.assign(V, {});
    parent.assign(V, -1);
    parentEdge.assign(V, -1);
    depth.assign(V, 0);

    buildTreeAdjFromBaum();
    prepareParentDepth();
}

void ContourMatrix::buildTreeAdjFromBaum() {
    const auto& baum = dec.getBaum();
    int V = (int)treeAdj.size();
    size_t added = 0, skipped = 0;
    for (const auto &t : baum) {
        int u = std::get<0>(t);
        int v = std::get<1>(t);
        int eid = std::get<2>(t);
        if (u < 0 || u >= V || v < 0 || v >= V) {
            std::cerr << "[Diag] Skipping Baum edge id=" << eid << " with out-of-range endpoints ("
                      << u << "," << v << ") for V=" << V << "\n";
            skipped++;
            continue;
        }
        treeAdj[u].push_back({v, eid});
        treeAdj[v].push_back({u, eid});
        added++;
    }
    std::cout << "[Diag] Baum edges processed: added=" << added << ", skipped=" << skipped
              << ", total input=" << baum.size() << "\n";
}

void ContourMatrix::prepareParentDepth() {
    int V = (int)treeAdj.size();
    std::vector<char> visited(V, 0);

    for (int s = 0; s < V; ++s) {
        if (visited[s]) continue;
        std::queue<int> q;
        visited[s] = 1;
        parent[s] = -1;
        parentEdge[s] = -1;
        depth[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto &p : treeAdj[u]) {
                int v = p.first;
                int eid = p.second;
                if (visited[v]) continue;
                visited[v] = 1;
                parent[v] = u;
                parentEdge[v] = eid;
                depth[v] = depth[u] + 1;
                q.push(v);
            }
        }
    }
}

std::vector<std::tuple<int,int,int>> ContourMatrix::getPathEdgesNodes(int u, int v) const {
    std::vector<std::tuple<int,int,int>> pathEdgesU, pathEdgesV;
    int a = u, b = v;

    if (a == b) return {};

    while (depth[a] > depth[b]) {
        if (a == -1) return {};
        int eid = parentEdge[a];
        int p = parent[a];
        if (p == -1) return {};
        pathEdgesU.emplace_back(eid, a, p);
        a = p;
    }
    while (depth[b] > depth[a]) {
        if (b == -1) return {};
        int eid = parentEdge[b];
        int p = parent[b];
        if (p == -1) return {};
        pathEdgesV.emplace_back(eid, b, p);
        b = p;
    }

    while (a != b) {
        if (parent[a] == -1 || parent[b] == -1) return {};
        int eidA = parentEdge[a];
        int pA = parent[a];
        pathEdgesU.emplace_back(eidA, a, pA);
        a = pA;

        int eidB = parentEdge[b];
        int pB = parent[b];
        pathEdgesV.emplace_back(eidB, b, pB);
        b = pB;
    }

    std::vector<std::tuple<int,int,int>> full;
    full.reserve(pathEdgesU.size() + pathEdgesV.size());
    for (auto &t : pathEdgesU) full.push_back(t);
    for (auto it = pathEdgesV.rbegin(); it != pathEdgesV.rend(); ++it) {
        int eid, fromN, toN;
        std::tie(eid, fromN, toN) = *it;
        full.emplace_back(eid, toN, fromN);
    }
    return full;
}

void ContourMatrix::build() {
    cycles.clear();
    cycleAntiEdgeIds.clear();

    // Rebuild tree adjacency and parent/depth using current Baum from dec
    int V = graph.numVertices();
    treeAdj.assign(V, {});
    parent.assign(V, -1);
    parentEdge.assign(V, -1);
    depth.assign(V, 0);
    buildTreeAdjFromBaum();
    prepareParentDepth();

    const auto& antibaum = dec.getAntibaum();
    const auto& edges = graph.getEdges();

    // Temporary diagnostic: compute connected components of the tree adjacency
    // and report component IDs for each anti-edge (u, v, id)
    {
        int V = (int)treeAdj.size();
        std::vector<int> compId(V, -1);
        int nextComp = 0;
        std::queue<int> q;
        for (int s = 0; s < V; ++s) {
            if (compId[s] != -1) continue;
            compId[s] = nextComp;
            q.push(s);
            while (!q.empty()) {
                int u0 = q.front(); q.pop();
                for (auto &p : treeAdj[u0]) {
                    int v0 = p.first;
                    if (compId[v0] != -1) continue;
                    compId[v0] = nextComp;
                    q.push(v0);
                }
            }
            nextComp++;
        }

        std::cout << "[Diag] Tree components computed: " << nextComp << "\n";
        for (const auto &t : antibaum) {
            int u = std::get<0>(t);
            int v = std::get<1>(t);
            int aid = std::get<2>(t);
            int cu = (u >= 0 && u < V) ? compId[u] : -1;
            int cv = (v >= 0 && v < V) ? compId[v] : -1;
            bool diff = (cu != cv);
            std::cout << "[Diag] anti-edge id=" << aid << ": (u=" << u << ", v=" << v
                      << ") comp(u)=" << cu << ", comp(v)=" << cv
                      << (diff ? " [DIFF]" : "") << "\n";
        }
    }

    std::unordered_map<int, std::pair<int,int>> edgeMap;
    edgeMap.reserve(edges.size());
    for (const auto &e : edges) edgeMap[e.id] = {e.from, e.to};

    cycles.reserve(antibaum.size());
    cycleAntiEdgeIds.reserve(antibaum.size());
    for (const auto &t : antibaum) {
        int u = std::get<0>(t);
        int v = std::get<1>(t);
        int aid = std::get<2>(t);

        int a_from = u, a_to = v;
        auto it = edgeMap.find(aid);
        if (it != edgeMap.end()) { a_from = it->second.first; a_to = it->second.second; }

        // Build path from anti-edge direction: a_from -> a_to
        std::vector<std::tuple<int,int,int>> path = getPathEdgesNodes(a_from, a_to);
        SparseCycle row;

        if (path.empty()) {
            std::cerr << "Error: no tree path found between " << a_from << " and " << a_to
                      << " for anti-edge id=" << aid << ". Skipping this cycle.\n";
            continue;
        }

        for (auto &pe : path) {
            int eid, fromN, toN;
            std::tie(eid, fromN, toN) = pe;
            auto it2 = edgeMap.find(eid);
            if (it2 != edgeMap.end()) {
                int orig_from = it2->second.first;
                int orig_to   = it2->second.second;
                if (orig_from == fromN && orig_to == toN) {
                    row[eid] = +1;
                } else if (orig_from == toN && orig_to == fromN) {
                    row[eid] = -1;
                } else {
                    std::cerr << "Error: tree edge id=" << eid << " endpoints (" << orig_from
                              << "," << orig_to << ") do not match path segment (" << fromN
                              << "," << toN << "). Skipping this edge in cycle for anti-edge id="
                              << aid << "\n";
                }
            } else {
                std::cerr << "Error: unknown tree edge id=" << eid << ". Skipping this edge in cycle for anti-edge id=" << aid << "\n";
            }
        }

        auto it3 = edgeMap.find(aid);
        if (it3 != edgeMap.end()) {
            int orig_from = it3->second.first;
            int orig_to   = it3->second.second;
            // To form a closed circulation, the anti-edge direction must close the tree path.
            // If the tree path goes a_from -> a_to, the anti-edge must be oriented a_to -> a_from.
            if (orig_from == a_from && orig_to == a_to) {
                row[aid] = -1; // invert orientation to close the cycle
            } else if (orig_from == a_to && orig_to == a_from) {
                row[aid] = +1; // already opposite to the path, keeps circulation balanced
            } else {
                std::cerr << "Error: anti-edge id=" << aid << " endpoints (" << orig_from
                          << "," << orig_to << ") do not match resolved endpoints (" << a_from
                          << "," << a_to << "). Skipping this cycle.\n";
                continue;
            }
        } else {
            std::cerr << "Error: unknown anti-edge id=" << aid << ". Skipping this cycle.\n";
            continue;
        }

        cycles.push_back(std::move(row));
        cycleAntiEdgeIds.push_back(aid);
    }
}

std::vector<std::vector<int>> ContourMatrix::toDenseMatrix() const {
    // Build explicit mapping from edge id to compact column index
    const auto &edges = graph.getEdges();
    std::vector<int> edgeIds;
    edgeIds.reserve(edges.size());
    for (const auto &e : edges) edgeIds.push_back(e.id);
    std::sort(edgeIds.begin(), edgeIds.end());
    edgeIds.erase(std::unique(edgeIds.begin(), edgeIds.end()), edgeIds.end());

    std::unordered_map<int,int> idToCol;
    idToCol.reserve(edgeIds.size());
    for (size_t i = 0; i < edgeIds.size(); ++i) idToCol[edgeIds[i]] = (int)i;

    std::vector<std::vector<int>> dense;
    dense.assign((int)cycles.size(), std::vector<int>((int)edgeIds.size(), 0));
    for (size_t r = 0; r < cycles.size(); ++r) {
        for (auto &kv : cycles[r]) {
            int eid = kv.first;
            int sign = kv.second;
            auto it = idToCol.find(eid);
            if (it != idToCol.end()) {
                dense[r][it->second] = sign;
            }
        }
    }
    return dense;
}

void ContourMatrix::printSparse() const {
    for (size_t i = 0; i < cycles.size(); ++i) {
        std::cout << "C" << i << ": ";
        for (auto &kv : cycles[i]) {
            std::cout << "(" << kv.first << ":" << kv.second << ") ";
        }
        std::cout << "\n";
    }
}

void ContourMatrix::printDense() const {
    auto D = toDenseMatrix();
    for (auto &row : D) {
        for (int v : row) std::cout << v << " ";
        std::cout << "\n";
    }
}

bool ContourMatrix::validate() const {
    // Basic shape agreement
    if (cycles.size() != cycleAntiEdgeIds.size()) {
        std::cerr << "[Validate] Mismatch: cycles rows=" << cycles.size()
                  << " vs anti-edge ids=" << cycleAntiEdgeIds.size() << "\n";
        return false;
    }

    // Build quick lookup sets/maps we need
    std::unordered_set<int> antiIds;
    for (const auto &t : dec.getAntibaum()) antiIds.insert(std::get<2>(t));
    std::unordered_set<int> treeIds;
    for (const auto &t : dec.getBaum()) treeIds.insert(std::get<2>(t));

    // All anti-edges must be represented by exactly one row
    if (cycles.size() != antiIds.size()) {
        std::cerr << "[Validate] Expected one row per anti-edge: rows=" << cycles.size()
                  << ", anti-edges=" << antiIds.size() << "\n";
        return false;
    }
    std::unordered_set<int> antiSeen;
    for (int id : cycleAntiEdgeIds) antiSeen.insert(id);
    if (antiSeen.size() != cycleAntiEdgeIds.size()) {
        std::cerr << "[Validate] Duplicate anti-edge ids across rows detected\n";
        return false;
    }
    if (antiSeen != antiIds) {
        std::cerr << "[Validate] The set of anti-edge ids in rows does not match decomposition antibaum ids\n";
        return false;
    }

    // Map of edge id -> (u,v)
    std::unordered_map<int, std::pair<int,int>> edgeMap;
    for (const auto &e : graph.getEdges()) edgeMap[e.id] = {e.from, e.to};

    // Validate each row
    for (size_t r = 0; r < cycles.size(); ++r) {
        const auto &row = cycles[r];
        const int rowAntiId = cycleAntiEdgeIds[r];

        if (row.empty()) {
            std::cerr << "[Validate] Row " << r << " is empty\n";
            return false;
        }

        int antiCount = 0;
        for (const auto &kv : row) {
            const int eid = kv.first;
            const int sgn = kv.second;
            if (sgn != -1 && sgn != +1) {
                std::cerr << "[Validate] Row " << r << ": invalid sign for edge " << eid << ": " << sgn << "\n";
                return false;
            }
            if (antiIds.count(eid)) {
                antiCount++;
                if (eid != rowAntiId) {
                    std::cerr << "[Validate] Row " << r << ": contains anti-edge " << eid
                              << " but basis anti-edge is " << rowAntiId << "\n";
                    return false;
                }
            } else if (!treeIds.count(eid)) {
                std::cerr << "[Validate] Row " << r << ": edge " << eid
                          << " is neither in Baum nor Antibaum\n";
                return false;
            }
        }
        if (antiCount != 1) {
            std::cerr << "[Validate] Row " << r << ": expected exactly one anti-edge, found " << antiCount << "\n";
            return false;
        }

        // Closed-walk check: B * c = 0 using incidence (+1 at head, -1 at tail) times coefficients
        std::unordered_map<int,int> divergence; // node -> net flow
        for (const auto &kv : row) {
            const int eid = kv.first;
            const int sgn = kv.second;
            auto it = edgeMap.find(eid);
            if (it == edgeMap.end()) {
                std::cerr << "[Validate] Row " << r << ": unknown edge id " << eid << "\n";
                return false;
            }
            int u = it->second.first;
            int v = it->second.second;
            // Column of incidence is -1 at u, +1 at v. Multiply by sgn and sum per node.
            divergence[u] -= sgn;
            divergence[v] += sgn;
        }
        for (const auto &kv : divergence) {
            if (kv.second != 0) {
                std::cerr << "[Validate] Row " << r << ": not a closed walk; node " << kv.first
                          << " has divergence " << kv.second << "\n";
                return false;
            }
        }
    }

    return true;
}
