#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <limits>
#include <queue>

class Graph {
public:
    // Структура для хранения ребра
    struct Edge {
        std::string to;
        double weight;

        Edge(const std::string& dest, double w) : to(dest), weight(w) {}

        bool operator>(const Edge& other) const {
            return weight > other.weight;
        }
    };

    // Конструктор
    Graph();

    // Добавить ребро (направленное или ненаправленное)
    void addEdge(const std::string& from, const std::string& to, double weight, bool bidirectional = true);

    // Добавить вершину
    void addVertex(const std::string& vertex);

    // Проверить существование вершины
    bool hasVertex(const std::string& vertex) const;

    // Получить количество вершин
    size_t getVertexCount() const;

    // Получить количество ребер
    size_t getEdgeCount() const;

    // Получить все вершины
    std::vector<std::string> getVertices() const;

    // Получить соседей вершины
    std::vector<Edge> getNeighbors(const std::string& vertex) const;

    // Алгоритм Дейкстры - найти кратчайший путь от start до end
    struct DijkstraResult {
        bool found;                                    // Найден ли путь
        double distance;                               // Расстояние
        std::vector<std::string> path;                 // Путь (список городов)
        std::map<std::string, double> distances;       // Расстояния до всех вершин от start

        DijkstraResult() : found(false), distance(std::numeric_limits<double>::infinity()) {}
    };

    DijkstraResult dijkstra(const std::string& start, const std::string& end) const;

    // Найти все кратчайшие пути от start до всех остальных вершин
    std::map<std::string, double> dijkstraAll(const std::string& start) const;

    // Вывести граф (для отладки)
    void print() const;

    // Очистить граф
    void clear();

private:
    // Список смежности: вершина -> список ребер
    std::unordered_map<std::string, std::vector<Edge>> adjacencyList;

    // Количество ребер
    size_t edgeCount;

    // Структура для приоритетной очереди в Дейкстре
    struct PQNode {
        std::string vertex;
        double distance;

        PQNode(const std::string& v, double d) : vertex(v), distance(d) {}

        bool operator>(const PQNode& other) const {
            return distance > other.distance;
        }
    };
};

#endif // GRAPH_H
