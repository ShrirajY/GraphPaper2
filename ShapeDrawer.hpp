#pragma once
#include <windows.h>
#include <cmath>
#include "Globals.hpp" // Assuming this header defines the constants like GraphWidth, GraphHeight, etc.
#include "Circle.hpp"
#include "Line.hpp"
#include "Ellipse.hpp"
class ShapeDrawer
{
public:
    ShapeDrawer(HDC hdc);

    void DrawPoint(float x, float y, COLORREF color, int size = DefaultPointSize) const;
    void DrawLine(Line* line) const;
    void DrawRectangle(float x1, float y1, float x2, float y2, COLORREF color) const;
    void DrawCircle(Circle* circle) const;
    void DrawEllipse(Ellipse_* ellipse) const;

    static constexpr int Scale = 25;
    static constexpr int DefaultPointSize = 5;
    static constexpr int LinePointSize = 7;
    static constexpr int EllipseSegments = 720;

private:
    HDC hdc_;
};


ShapeDrawer::ShapeDrawer(HDC hdc) : hdc_(hdc) {}

void ShapeDrawer::DrawPoint(float x, float y, COLORREF color, int size) const
{
    int px = static_cast<int>(x * Scale);
    int py = static_cast<int>(y * Scale);
    int half = size / 2;
    for (int dx = -half; dx <= half; ++dx)
    {
        for (int dy = -half; dy <= half; ++dy)
        {
            SetPixel(hdc_, px + dx, py + dy, color);
        }
    }
}

void ShapeDrawer::DrawLine(Line* line) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, line->color_);
    HGDIOBJ oldPen = SelectObject(hdc_, hPen);

    int startX = static_cast<int>(line->x1_ * Scale);
    int startY = static_cast<int>(line->y1_ * Scale);
    int endX = static_cast<int>(line->x2_ * Scale);
    int endY = static_cast<int>(line->y2_ * Scale);

    DrawPoint(line->x1_, line->y1_, line->color_, LinePointSize);
    DrawPoint(line->x2_, line->y2_, line->color_, LinePointSize);

    MoveToEx(hdc_, startX, startY, NULL);
    LineTo(hdc_, endX, endY);

    SelectObject(hdc_, oldPen);
    DeleteObject(hPen);
}

void ShapeDrawer::DrawRectangle(float x1, float y1, float x2, float y2, COLORREF color) const
{
    Line l1 = Line::Create(x1, y1, x2, y1, color);
    Line l2 = Line::Create(x2, y1, x2, y2, color);
    Line l3 = Line::Create(x2, y2, x1, y2, color);
    Line l4 = Line::Create(x1, y2, x1, y1, color);

    DrawLine(&l1);
    DrawLine(&l2);
    DrawLine(&l3);
    DrawLine(&l4);
}

void ShapeDrawer::DrawCircle(Circle* circle) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, circle->color_);
    HGDIOBJ oldPen = SelectObject(hdc_, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc_, GetStockObject(HOLLOW_BRUSH));

    int centerX = static_cast<int>(circle->centerX_ * Scale);
    int centerY = static_cast<int>(circle->centerY_ * Scale);
    int radius = static_cast<int>(circle->radius_ * Scale);

    Ellipse(hdc_, centerX - radius, centerY + radius, centerX + radius, centerY - radius);

    // DrawPoint(circle->centerX_, circle->centerY_, circle->color_, DefaultPointSize);

    SelectObject(hdc_, oldBrush);
    SelectObject(hdc_, oldPen);
    DeleteObject(hPen);
}

void ShapeDrawer::DrawEllipse(Ellipse_* ellipse) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, ellipse->color_);
    HGDIOBJ oldPen = SelectObject(hdc_, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc_, GetStockObject(HOLLOW_BRUSH));

    POINT pts[EllipseSegments];
    float rad = ellipse->angle_ * 3.14159265f / 180.0f;
    float cx = ellipse->centerX_ * Scale;
    float cy = ellipse->centerY_ * Scale;
    float ax = ellipse->a_ * Scale;
    float by = ellipse->b_ * Scale;

    for (int i = 0; i < EllipseSegments; i++)
    {
        float theta = i * 2.0f * 3.14159265f / EllipseSegments;
        float x = ax * cosf(theta);
        float y = by * sinf(theta);
        float xr = x * cosf(rad) - y * sinf(rad);
        float yr = x * sinf(rad) + y * cosf(rad);
        pts[i].x = static_cast<LONG>(cx + xr);
        pts[i].y = static_cast<LONG>(cy - yr); // Invert Y for device coordinates
    }

    Polyline(hdc_, pts, EllipseSegments);

    SelectObject(hdc_, oldBrush);
    SelectObject(hdc_, oldPen);
    DeleteObject(hPen);
}