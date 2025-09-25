#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <string>
#include <unordered_map>

class Color {
public:
    Color() : r_(0), g_(0), b_(0) {}          // <-- default constructor
    Color(int r, int g, int b) : r_(r), g_(g), b_(b) {}

    int getR() const { return r_; }
    int getG() const { return g_; }
    int getB() const { return b_; }

private:
    int r_;
    int g_;
    int b_;
};

class ColorsSymbolTables {
public:
    ColorsSymbolTables() {
        colorMap.emplace("__red__", Color(255, 0, 0));
        colorMap.emplace("__green__", Color(0, 255, 0));
        colorMap.emplace("__blue__", Color(0, 0, 255));
        colorMap.emplace("__black__", Color(0, 0, 0));
        colorMap.emplace("__white__", Color(255, 255, 255));
        colorMap.emplace("__yellow__", Color(255, 255, 0));
    }

    const Color& getColor(const std::string& name) const {
        auto it = colorMap.find(name);
        if (it != colorMap.end()) return it->second;
        throw std::invalid_argument("Color not found: " + name);
    }

    void addColor(const std::string& name, const Color& color) {
        colorMap[name] = color; // overwrite if exists
    }

    void clear() {
        this->colorMap.clear();
        // Re-add default colors
        colorMap.emplace("__red__", Color(255, 0, 0));
        colorMap.emplace("__green__", Color(0, 255, 0));
        colorMap.emplace("__blue__", Color(0, 0, 255));
        colorMap.emplace("__black__", Color(0, 0, 0));
        colorMap.emplace("__white__", Color(255, 255, 255));
        colorMap.emplace("__yellow__", Color(255, 255, 0));
    }
private:
    std::unordered_map<std::string, Color> colorMap;
};

ColorsSymbolTables globalColorsSymbolTable;


#endif // COLOR_H
