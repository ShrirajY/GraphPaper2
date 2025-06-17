#ifndef LINE_HPP
#define LINE_HPP

#include <windows.h>
#include "Globals.hpp"

class Line
{
public:
    Line(HDC hdc, float x1, float y1, float x2, float y2, COLORREF color = RGB(0, 0, 0));
    friend class ShapeDrawer;
    static Line Create(HDC hdc, float x1, float y1, float x2, float y2, COLORREF color);
private:
    HDC hdc_;
    float x1_;
    float y1_;
    float x2_;
    float y2_;
    COLORREF color_;
};


Line::Line(HDC hdc, float x1, float y1, float x2, float y2, COLORREF color)
    : hdc_(hdc), x1_(x1), y1_(y1), x2_(x2), y2_(y2), color_(color) {}

Line Line::Create(HDC hdc, float x1, float y1, float x2, float y2, COLORREF color)
{
    return Line(hdc, x1, y1, x2, y2, color);
}
#endif // LINE_HPP