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

#endif // ELLIPSE_HPP