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

    bool operator==(const Parabola& other) const
    {
        return (x1_ == other.x1_ && y1_ == other.y1_ &&
                vx_ == other.vx_ && vy_ == other.vy_ &&
                x2_ == other.x2_ && y2_ == other.y2_);
    }

    bool operator!=(const Parabola& other) const { return !(*this == other); }

    friend class ShapeDrawer;
    friend class ShapeManager;
    friend class ShowShapesInfo;

private:
    float x1_, y1_;
    float vx_, vy_;
    float x2_, y2_;
    COLORREF color_;
};

// --- Constructor ---

Parabola::Parabola(float x1, float y1, float vx, float vy, float x2, float y2, COLORREF color)
    : x1_(x1), y1_(y1), vx_(vx), vy_(vy), x2_(x2), y2_(y2), color_(color) {}

// --- Factory Function ---

Parabola Parabola::CreateFrom3Points(float x1, float y1, float vx, float vy, float x2, float y2, COLORREF color)
{
    return Parabola(x1, y1, vx, vy, x2, y2, color);
}

// --- Global Storage ---

std::list<Parabola> parabolaList;

void AddParabola(const Parabola& parabola)
{
    parabolaList.push_back(parabola);
}

void removeParabola(const Parabola& parabola)
{
    parabolaList.remove(parabola);
}

Parabola* SelectedParabola = nullptr;

#endif // PARABOLA_HPP
