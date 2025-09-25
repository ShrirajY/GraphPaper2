#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <string>
#include <unordered_map>

class Vertex
{
public:
    Vertex(float x, float y) : x_(x), y_(y) {}
    float getX() const { return x_; }
    float getY() const { return y_; }

private:
    float x_;
    float y_;
};

class VertexSymbolTables
{
public:
    Vertex getVertex(const std::string &name) const
    {
        auto it = vertexMap.find(name);
        if (it != vertexMap.end())
        {
            return it->second;
        }
        throw std::runtime_error("Vertex not found: " + name);
    }

    void addVertex(const std::string &name, const Vertex &vertex)
    {
        this->vertexMap.emplace(name, vertex);
    }

    void clear()
    {
        this->vertexMap.clear();
    }

private:
    std::unordered_map<std::string, Vertex> vertexMap;
};

VertexSymbolTables globalVertexSymbolTable;

#endif // VERTEX_H