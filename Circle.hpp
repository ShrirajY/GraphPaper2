#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <windows.h>
#include "Globals.hpp"
#include <list>
class Circle
{
public:
    Circle(float centerX, float centerY, float radius, COLORREF color);

    static Circle Create(float centerX, float centerY, float radius, COLORREF color);

    bool operator==(const Circle& other) const
    {
        return (centerX_ == other.centerX_ && centerY_ == other.centerY_ && radius_ == other.radius_);
    }

    bool operator!=(const Circle& other) const
    {
        return !(*this == other);
    }

    void SetColor(COLORREF color) { color_ = color; }

    COLORREF GetColor() const { return color_; }
    float GetCenterX() const { return centerX_; }
    float GetCenterY() const { return centerY_; }
    float GetRadius() const { return radius_; }
    friend class ShapeDrawer;
    friend class ShapeManager;
    friend class ShowShapesInfo;
private:
    float centerX_;
    float centerY_;
    float radius_;
    COLORREF color_;
};

Circle::Circle(float centerX, float centerY, float radius, COLORREF color)
    : centerX_(centerX), centerY_(centerY), radius_(radius), color_(color) {}

Circle Circle::Create(float centerX, float centerY, float radius, COLORREF color)
{
    return Circle(centerX, centerY, radius, color);
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

Circle *SelectedCircle = nullptr;

#endif // CIRCLE_HPP