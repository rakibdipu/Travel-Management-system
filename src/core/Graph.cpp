#include "../../include/core/Graph.h"
#include "../../include/core/Utils.h"
#include "../../include/core/Exceptions.h"

Graph::Graph() {}

void Graph::addLocation(const std::string& name) {
    std::string trimmed = Utils::trim(name);
    if (trimmed.empty()) return;
    if (!hasLocation(trimmed)) {
        cityList.push_back(trimmed);
        adjList[trimmed] = {};
    }
}

bool Graph::hasLocation(const std::string& name) const {
    return adjList.find(name) != adjList.end();
}

void Graph::addConnection(const std::string& from, const std::string& to, int distance) {
    if (from.empty() || to.empty() || distance <= 0) return;
    addLocation(from);
    addLocation(to);

    // Check if edge already exists to prevent duplicates
    for (auto& edge : adjList[from]) {
        if (edge.to == to) {
            edge.distance = distance;
            for (auto& backEdge : adjList[to]) {
                if (backEdge.to == from) {
                    backEdge.distance = distance;
                    return;
                }
            }
            return;
        }
    }

    adjList[from].push_back({to, distance});
    adjList[to].push_back({from, distance});
}

bool Graph::hasConnection(const std::string& from, const std::string& to) const {
    auto it = adjList.find(from);
    if (it == adjList.end()) return false;
    for (const auto& edge : it->second) {
        if (edge.to == to) return true;
    }
    return false;
}

const std::vector<std::string>& Graph::getLocations() const {
    return cityList;
}

const std::vector<Edge>& Graph::getNeighbors(const std::string& city) const {
    static const std::vector<Edge> empty;
    auto it = adjList.find(city);
    if (it != adjList.end()) return it->second;
    return empty;
}

const std::unordered_map<std::string, std::vector<Edge>>& Graph::getAllConnections() const {
    return adjList;
}

DijkstraResult Graph::findShortestPath(const std::string& start) const {
    if (!hasLocation(start)) {
        throw LocationNotFoundException(start);
    }

    DijkstraResult result;
    result.startCity = start;

    for (const auto& city : cityList) {
        result.distances[city] = std::numeric_limits<int>::max();
    }
    result.distances[start] = 0;

    // Pair of (distance, city_name)
    typedef std::pair<int, std::string> DistPair;
    std::priority_queue<DistPair, std::vector<DistPair>, std::greater<DistPair>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int d = pq.top().first;
        std::string u = pq.top().second;
        pq.pop();

        if (d > result.distances[u]) continue;

        auto it = adjList.find(u);
        if (it == adjList.end()) continue;

        for (const auto& edge : it->second) {
            const std::string& v = edge.to;
            int weight = edge.distance;

            if (result.distances[u] + weight < result.distances[v]) {
                result.distances[v] = result.distances[u] + weight;
                result.parent[v] = u;
                pq.push({result.distances[v], v});
            }
        }
    }

    return result;
}

void Graph::displayNetwork() const {
    std::cout << Utils::Color::BOLD_CYAN << "\n[+] Current Highway & City Route Network:" << Utils::Color::RESET << "\n";
    Utils::printDivider(74);
    std::cout << std::left << std::setw(6) << "No." 
              << std::setw(20) << "City / Node" 
              << "Connected Direct Neighbors (Distance in KM)\n";
    Utils::printDivider(74);

    for (size_t i = 0; i < cityList.size(); ++i) {
        const std::string& city = cityList[i];
        std::cout << std::left << std::setw(6) << (i + 1)
                  << std::setw(20) << city;
        
        const auto& edges = getNeighbors(city);
        if (edges.empty()) {
            std::cout << Utils::Color::DIM << "(No direct roads)" << Utils::Color::RESET;
        } else {
            for (size_t j = 0; j < edges.size(); ++j) {
                std::cout << edges[j].to << " (" << edges[j].distance << "km)";
                if (j + 1 < edges.size()) std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
    Utils::printDivider(74);
}

void Graph::clear() {
    cityList.clear();
    adjList.clear();
}
