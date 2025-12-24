#include "graph.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

Graph::Graph() : edgeCount(0) {}

void Graph::addVertex(const std::string& vertex) {
    if (adjacencyList.find(vertex) == adjacencyList.end()) {
        adjacencyList[vertex] = std::vector<Edge>();
    }
}

void Graph::addEdge(const std::string& from, const std::string& to, double weight, bool bidirectional) {
    // Добавляем вершины, если их еще нет
    addVertex(from);
    addVertex(to);

    // Добавляем ребро from -> to
    adjacencyList[from].push_back(Edge(to, weight));
    edgeCount++;

    // Если граф ненаправленный, добавляем обратное ребро
    if (bidirectional) {
        adjacencyList[to].push_back(Edge(from, weight));
        edgeCount++;
    }
}

bool Graph::hasVertex(const std::string& vertex) const {
    return adjacencyList.find(vertex) != adjacencyList.end();
}

size_t Graph::getVertexCount() const {
    return adjacencyList.size();
}

size_t Graph::getEdgeCount() const {
    return edgeCount;
}

std::vector<std::string> Graph::getVertices() const {
    std::vector<std::string> vertices;
    for (const auto& pair : adjacencyList) {
        vertices.push_back(pair.first);
    }
    return vertices;
}

std::vector<Graph::Edge> Graph::getNeighbors(const std::string& vertex) const {
    auto it = adjacencyList.find(vertex);
    if (it != adjacencyList.end()) {
        return it->second;
    }
    return std::vector<Edge>();
}

Graph::DijkstraResult Graph::dijkstra(const std::string& start, const std::string& end) const {
    DijkstraResult result;

    // Проверяем существование вершин
    if (!hasVertex(start) || !hasVertex(end)) {
        return result;
    }

    // Инициализация
    std::map<std::string, double> distances;
    std::map<std::string, std::string> previous;
    std::set<std::string> visited;

    // Приоритетная очередь (min-heap)
    std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;

    // Устанавливаем начальные расстояния
    for (const auto& pair : adjacencyList) {
        distances[pair.first] = std::numeric_limits<double>::infinity();
    }
    distances[start] = 0.0;

    // Добавляем стартовую вершину в очередь
    pq.push(PQNode(start, 0.0));

    // Основной цикл Дейкстры
    while (!pq.empty()) {
        PQNode current = pq.top();
        pq.pop();

        std::string currentVertex = current.vertex;

        // Если уже посетили эту вершину, пропускаем
        if (visited.find(currentVertex) != visited.end()) {
            continue;
        }

        visited.insert(currentVertex);

        // Если достигли конечной вершины, можно остановиться
        if (currentVertex == end) {
            break;
        }

        // Просматриваем всех соседей
        auto neighbors = getNeighbors(currentVertex);
        for (const auto& edge : neighbors) {
            if (visited.find(edge.to) != visited.end()) {
                continue;
            }

            double newDistance = distances[currentVertex] + edge.weight;

            if (newDistance < distances[edge.to]) {
                distances[edge.to] = newDistance;
                previous[edge.to] = currentVertex;
                pq.push(PQNode(edge.to, newDistance));
            }
        }
    }

    // Формируем результат
    result.distances = distances;
    result.distance = distances[end];

    if (distances[end] != std::numeric_limits<double>::infinity()) {
        result.found = true;

        // Восстанавливаем путь
        std::vector<std::string> path;
        std::string current = end;

        while (current != start) {
            path.push_back(current);
            auto it = previous.find(current);
            if (it == previous.end()) {
                break;
            }
            current = it->second;
        }
        path.push_back(start);

        // Переворачиваем путь (сейчас он от end к start)
        std::reverse(path.begin(), path.end());
        result.path = path;
    }

    return result;
}

void Graph::print() const {
    std::cout << "\nГраф (вершин: " << getVertexCount()
              << ", ребер: " << edgeCount << "):" << std::endl;

    for (const auto& pair : adjacencyList) {
        std::cout << "  " << pair.first << " -> ";
        const auto& edges = pair.second;

        if (edges.empty()) {
            std::cout << "(нет соседей)";
        } else {
            for (size_t i = 0; i < edges.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << edges[i].to << " (" << edges[i].weight << ")";
            }
        }
        std::cout << std::endl;
    }
}

void Graph::clear() {
    adjacencyList.clear();
    edgeCount = 0;
}
