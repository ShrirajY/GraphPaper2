#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include <windows.h>
#include "Globals.hpp"
#include <list>
class Ellipse_
{
public:
    Ellipse_(HDC hdc, float centerX, float centerY, float a, float b, float angle, COLORREF color);

    static Ellipse_ Create(HDC hdc, float centerX, float centerY, float a, float b, float angle, COLORREF color);

    void setColor(COLORREF color)
    {
        this->color_ = color;
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
private:
    HDC hdc_;
    float centerX_;
    float centerY_;
    float a_;
    float b_;
    float angle_;
    COLORREF color_;
};

Ellipse_::Ellipse_(HDC hdc, float centerX, float centerY, float a, float b, float angle, COLORREF color)
    : hdc_(hdc), centerX_(centerX), centerY_(centerY), a_(a), b_(b), angle_(angle), color_(color) {}

Ellipse_ Ellipse_::Create(HDC hdc, float centerX, float centerY, float a, float b, float angle, COLORREF color)
{
    return Ellipse_(hdc, centerX, centerY, a, b, angle, color);
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

#endif // ELLIPSE_HPP