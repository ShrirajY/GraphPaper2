#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include <windows.h>
#include "Globals.hpp"
#include <list>
class Ellipse_
{
public:
    Ellipse_(float centerX, float centerY, float a, float b, float angle, COLORREF color);

    static Ellipse_ Create(float centerX, float centerY, float a, float b, float angle, COLORREF color);

    void setColor(COLORREF color)
    {
        this->color_ = color;
    }

    COLORREF getColor() const
    {
        return color_;
    }
    float getCenterX() const
    {
        return centerX_;
    }
    float getCenterY() const
    {
        return centerY_;
    }
    float getA() const
    {
        return a_;
    }
    float getB() const
    {
        return b_;
    }
    float getAngle() const
    {
        return angle_;
    }
    bool operator==(const Ellipse_& other) const
    {
        return (centerX_ == other.centerX_ && centerY_ == other.centerY_ &&
                a_ == other.a_ && b_ == other.b_ && angle_ == other.angle_);
    }

    bool operator!=(const Ellipse_& other) const
    {
        return !(*this == other);
    }
    friend class ShapeDrawer;
    friend class ShapeManager;
    friend class ShowShapesInfo;

private:
    float centerX_;
    float centerY_;
    float a_;
    float b_;
    float angle_;
    COLORREF color_;
};

Ellipse_::Ellipse_(float centerX, float centerY, float a, float b, float angle, COLORREF color)
    : centerX_(centerX), centerY_(centerY), a_(a), b_(b), angle_(angle), color_(color) {}

Ellipse_ Ellipse_::Create(float centerX, float centerY, float a, float b, float angle, COLORREF color)
{
    return Ellipse_(centerX, centerY, a, b, angle, color);
}

std::list<Ellipse_> ellipseList;

void AddEllipse(const Ellipse_& ellipse)
{
    ellipseList.push_back(ellipse);
}

void removeEllipse(const Ellipse_& ellipse)
{
    ellipseList.remove(ellipse);
}

Ellipse_ *SelectedEllipse = nullptr;

#endif // ELLIPSE_HPP