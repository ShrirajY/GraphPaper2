#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <windows.h>
#include "Globals.hpp"
#include <list>
class Circle
{
public:
    Circle(HDC hdc, float centerX, float centerY, float radius, COLORREF color);

    static Circle Create(HDC hdc, float centerX, float centerY, float radius, COLORREF color);
    friend class ShapeDrawer;
private:
    HDC hdc_;
    float centerX_;
    float centerY_;
    float radius_;
    COLORREF color_;
};

Circle::Circle(HDC hdc, float centerX, float centerY, float radius, COLORREF color)
    : hdc_(hdc), centerX_(centerX), centerY_(centerY), radius_(radius), color_(color) {}

Circle Circle::Create(HDC hdc, float centerX, float centerY, float radius, COLORREF color)
{
    return Circle(hdc, centerX, centerY, radius, color);
}

std::list<Circle> CircleList;
void AddCircle(const Circle& circle)
{
    CircleList.push_back(circle);
}

void removeCircle(const Circle& circle)
{
    CircleList.remove(circle);
}

#endif // CIRCLE_HPP