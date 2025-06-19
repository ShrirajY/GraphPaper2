#ifndef LINE_HPP
#define LINE_HPP

#include <windows.h>
#include "Globals.hpp"
#include <list>
class Line
{
public:
    Line(float x1, float y1, float x2, float y2, COLORREF color = RGB(0, 0, 0));
    friend class ShapeDrawer;
    static Line Create(float x1, float y1, float x2, float y2, COLORREF color);

    void setColor(COLORREF color)
    {
        this->color_ = color;
    }

    bool operator==(const Line& other) const
    {
        return (x1_ == other.x1_ && y1_ == other.y1_ && x2_ == other.x2_ && y2_ == other.y2_);
    }

    bool operator!=(const Line& other) const
    {
        return !(*this == other);
    }

private:
    float x1_;
    float y1_;
    float x2_;
    float y2_;
    COLORREF color_;
};


Line::Line(float x1, float y1, float x2, float y2, COLORREF color)
    : x1_(x1), y1_(y1), x2_(x2), y2_(y2), color_(color) {}

Line Line::Create(float x1, float y1, float x2, float y2, COLORREF color)
{
    return Line(x1, y1, x2, y2, color);
}

std::list<Line> lineList;

void AddLine(const Line& line)
{
    lineList.push_back(line);
}

void removeLine(const Line& line)
{
    lineList.remove(line);
}

#endif // LINE_HPP