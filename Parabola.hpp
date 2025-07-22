#ifndef PARABOLA_HPP
#define PARABOLA_HPP

#include <windows.h>
#include <list>
#include "Globals.hpp"

class Parabola
{
public:
    Parabola(float x1, float y1, float vx, float vy, float x2, float y2, COLORREF color);

    static Parabola CreateFrom3Points(float x1, float y1, float vx, float vy, float x2, float y2, COLORREF color);

    void setColor(COLORREF color) { color_ = color; }
    COLORREF getColor() const { return color_; }

    float getX1() const { return x1_; }
    float getY1() const { return y1_; }

    float getVertexX() const { return vx_; }
    float getVertexY() const { return vy_; }

    float getX2() const { return x2_; }
    float getY2() const { return y2_; }

    bool operator==(const Parabola &other) const
    {
        return (x1_ == other.x1_ && y1_ == other.y1_ &&
                vx_ == other.vx_ && vy_ == other.vy_ &&
                x2_ == other.x2_ && y2_ == other.y2_);
    }

    bool operator!=(const Parabola &other) const { return !(*this == other); }

    friend class ShapeDrawer;
    friend class ShapeManager;
    friend class ShowShapesInfo;

private:
    float x1_, y1_;
    float vx_, vy_;
    float x2_, y2_;
    float vertexX_, vertexY_;
    float focusX_, focusY_;
    COLORREF color_;
};

// --- Constructor ---

Parabola::Parabola(float x1, float y1, float vx, float vy, float x2, float y2, COLORREF color)
    : x1_(x1), y1_(y1), vx_(vx), vy_(vy), x2_(x2), y2_(y2), color_(color)
{
    // Step 1: Vertex is turning point
    vertexX_ = vx;
    vertexY_ = vy;

    // Step 2: Midpoint of chord connecting the endpoints
    float mx = (x1 + x2) / 2.0f;
    float my = (y1 + y2) / 2.0f;

    // Step 3: Vector from vertex to midpoint of chord = axis direction
    float ax = mx - vx;
    float ay = my - vy;

    // Step 4: Normalize axis
    float len = std::sqrt(ax * ax + ay * ay);
    if (len < 1e-6f) {
        // Edge case: degenerate parabola — endpoints and vertex collapsed
        focusX_ = vx;
        focusY_ = vy;
        return;
    }

    ax /= len;
    ay /= len;

    // Step 5: Calculate distance from vertex to chord (height)
    float dx = x1 - vx;
    float dy = y1 - vy;
    float height = std::abs(dx * (-ay) + dy * ax);  // projection onto normal

    // Step 6: Focus lies along the axis of symmetry from vertex at distance height
    focusX_ = vx + ax * height;
    focusY_ = vy + ay * height;
}


// --- Factory Function ---

Parabola Parabola::CreateFrom3Points(float x1, float y1, float vx, float vy, float x2, float y2, COLORREF color)
{
    return Parabola(x1, y1, vx, vy, x2, y2, color);
}

// --- Global Storage ---

std::list<Parabola> parabolaList;

void AddParabola(const Parabola &parabola)
{
    parabolaList.push_back(parabola);
}

void removeParabola(const Parabola &parabola)
{
    parabolaList.remove(parabola);
}

Parabola *SelectedParabola = nullptr;

#endif // PARABOLA_HPP
