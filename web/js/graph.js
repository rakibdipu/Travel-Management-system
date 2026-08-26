/**
 * Graph Data Structure and Dijkstra's Shortest Path Algorithm for Web UI
 */

class GraphNetwork {
    constructor() {
        this.adjList = new Map();
        this.cityNodes = new Map(); // stores coordinates and metadata
    }

    addCity(name, x, y, region, image, tagline, attractions) {
        if (!this.adjList.has(name)) {
            this.adjList.set(name, []);
            this.cityNodes.set(name, {
                name,
                x: x || 400,
                y: y || 450,
                region: region || "General",
                image: image || "https://images.unsplash.com/photo-1596895111956-bf1cf0599ce5?w=1600&q=85",
                tagline: tagline || "Scenic Destination of Bangladesh",
                attractions: attractions || ["Historic Landmarks", "Scenic Riverfront", "Cultural Heritage"]
            });
        }
    }

    addConnection(from, to, distance) {
        this.addCity(from);
        this.addCity(to);

        // Check if edge already exists
        const fromEdges = this.adjList.get(from);
        const existing = fromEdges.find(e => e.to === to);
        if (existing) {
            existing.distance = distance;
            const toEdges = this.adjList.get(to);
            const backEdge = toEdges.find(e => e.to === from);
            if (backEdge) backEdge.distance = distance;
            return;
        }

        this.adjList.get(from).push({ to, distance });
        this.adjList.get(to).push({ to: from, distance });
    }

    getCities() {
        return Array.from(this.cityNodes.values());
    }

    getConnections() {
        const edges = [];
        const seen = new Set();
        for (const [from, neighbors] of this.adjList.entries()) {
            for (const { to, distance } of neighbors) {
                const key = [from, to].sort().join("---");
                if (!seen.has(key)) {
                    seen.add(key);
                    edges.push({ from, to, distance });
                }
            }
        }
        return edges;
    }

    findShortestPath(startCity, destCity) {
        if (!this.adjList.has(startCity) || !this.adjList.has(destCity)) {
            return { hasPath: false, error: "One or both cities not found in network." };
        }

        const distances = {};
        const parent = {};
        const visited = new Set();

        for (const city of this.adjList.keys()) {
            distances[city] = Infinity;
        }
        distances[startCity] = 0;

        // Priority Queue simulation using sorted array
        const pq = [{ city: startCity, dist: 0 }];

        while (pq.length > 0) {
            pq.sort((a, b) => a.dist - b.dist);
            const { city: u, dist: d } = pq.shift();

            if (visited.has(u)) continue;
            visited.add(u);

            if (u === destCity) break;

            const neighbors = this.adjList.get(u) || [];
            for (const edge of neighbors) {
                const v = edge.to;
                const weight = edge.distance;

                if (!visited.has(v) && distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    parent[v] = u;
                    pq.push({ city: v, dist: distances[v] });
                }
            }
        }

        if (distances[destCity] === Infinity) {
            return { hasPath: false, error: "No viable highway path found between selected locations." };
        }

        // Reconstruct exact path
        const path = [];
        let curr = destCity;
        while (curr) {
            path.push(curr);
            if (curr === startCity) break;
            curr = parent[curr];
        }
        path.reverse();

        return {
            hasPath: true,
            startCity,
            destCity,
            totalDistance: distances[destCity],
            path: path
        };
    }

    calculateTransportOptions(distanceKm, transportModes) {
        return transportModes.map(t => {
            let fare = distanceKm * t.rate;
            if (t.type === "DOMESTIC_FLIGHT") {
                fare = (t.baseSurcharge || 2500) + (distanceKm * t.rate * 0.75);
            }
            const hours = distanceKm / t.speed;
            const h = Math.floor(hours);
            const m = Math.round((hours - h) * 60);
            const durationStr = (h > 0 ? `${h}h ` : "") + `${m}m`;

            return {
                ...t,
                fare: Math.round(fare * 100) / 100,
                durationHours: hours,
                durationString: durationStr
            };
        });
    }
}
