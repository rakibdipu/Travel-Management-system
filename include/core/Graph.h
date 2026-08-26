#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>

struct Edge {
    std::string to;
    int distance; // in km
};

struct DijkstraResult {
    std::string startCity;
    std::unordered_map<std::string, int> distances;
    std::unordered_map<std::string, std::string> parent;

    bool hasPath(const std::string& destination) const {
        auto it = distances.find(destination);
        return it != distances.end() && it->second < std::numeric_limits<int>::max();
    }

    int getDistance(const std::string& destination) const {
        auto it = distances.find(destination);
        if (it != distances.end()) return it->second;
        return -1;
    }

    std::vector<std::string> getPath(const std::string& destination) const {
        std::vector<std::string> path;
        if (!hasPath(destination)) return path;

        std::string curr = destination;
        while (!curr.empty()) {
            path.push_back(curr);
            if (curr == startCity) break;
            auto it = parent.find(curr);
            if (it != parent.end()) {
                curr = it->second;
            } else {
                break;
            }
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};

class Graph {
private:
    std::vector<std::string> cityList;
    std::unordered_map<std::string, std::vector<Edge>> adjList;

public:
    Graph();

    void addLocation(const std::string& name);
    bool hasLocation(const std::string& name) const;
    void addConnection(const std::string& from, const std::string& to, int distance);
    bool hasConnection(const std::string& from, const std::string& to) const;

    const std::vector<std::string>& getLocations() const;
    const std::vector<Edge>& getNeighbors(const std::string& city) const;
    const std::unordered_map<std::string, std::vector<Edge>>& getAllConnections() const;

    DijkstraResult findShortestPath(const std::string& start) const;
    void displayNetwork() const;
    void clear();
};

#endif // GRAPH_H
