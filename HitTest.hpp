// ShapeManager.hpp
#pragma once
#include <list>
#include <cmath>
#include <string>
#include <windows.h>
#include "Line.hpp"
#include "Circle.hpp"
#include "Ellipse.hpp"

class ShapeManager
{
public:
    // Hit test: returns "Line", "Circle", "Ellipse", or "None"
    std::string HitTest(float x, float y, float tol = 5.0f) const
    {
        // Check lines
        for (auto &line : lineList)
        {
            if (IsPointOnLineSegment(x, y, line.x1_, line.y1_, line.x2_, line.y2_, tol))
            {
                line.setColor(RGB(255, 255, 0));   // Change color to yellow for hit
                InvalidateRect(hMain, NULL, TRUE); // Redraw the window
                MessageBox(NULL, TEXT("Line selected"), TEXT("Hit Test"), MB_OK);
                return "Line";
            }
        }
        // Check circles
        for (auto &circle : CircleList)
        {
            if (IsPointOnCircle(x, y, circle.centerX_, circle.centerY_, circle.radius_, tol))
            {
                circle.SetColor(RGB(255, 255, 0)); // Change color to yellow for hit
                InvalidateRect(hMain, NULL, TRUE); // Redraw the window
                MessageBox(NULL, TEXT("Circle selected"), TEXT("Hit Test"), MB_OK);
                return "Circle";
            }
        }
        // Check ellipses
        for (const auto &ellipse : ellipseList)
        {
            if (IsPointOnEllipse(x, y, ellipse.centerX_, ellipse.centerY_, ellipse.a_, ellipse.b_, tol, ellipse.angle_))
            {
                MessageBox(NULL, TEXT("Ellipse selected"), TEXT("Hit Test"), MB_OK);
                return "Ellipse";
            }
        }
        return "None";
    }

private:
    std::list<Line> lines_;
    std::list<Circle> circles_;
    std::list<Ellipse_> ellipses_;

    // Helper functions
    static bool IsPointOnLineSegment(float x, float y, float x1, float y1, float x2, float y2, float tol)
    {
        float dx = x2 - x1, dy = y2 - y1;
        float len_sq = dx * dx + dy * dy;
        if (len_sq == 0.0f)
            return (std::hypot(x - x1, y - y1) <= tol);
        float t = ((x - x1) * dx + (y - y1) * dy) / len_sq;
        if (t < 0.0f || t > 1.0f)
            return false;
        float proj_x = x1 + t * dx;
        float proj_y = y1 + t * dy;
        return (std::hypot(x - proj_x, y - proj_y) <= tol);
    }
    static bool IsPointOnCircle(float x, float y, float cx, float cy, float r, float tol)
    {
        float dist = std::hypot(x - cx, y - cy);
        return std::abs(dist - r) <= tol;
    }
    static bool IsPointOnEllipse(float x, float y, float cx, float cy, float rx, float ry, float tol, float angleDeg = 0.0f)
    {
        // Convert angle from degrees to radians
        float angleRad = angleDeg * 3.14159265f / 180.0f;

        // Translate point to ellipse center
        float dx = x - cx;
        float dy = y - cy;

        // Rotate point by -angleRad (counterclockwise)
        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);
        float Xp = dx * cosA - dy * sinA;
        float Yp = dx * sinA + dy * cosA;

        float val = (Xp * Xp) / (rx * rx) + (Yp * Yp) / (ry * ry);
        return std::abs(val - 1.0f) <= tol;
    }
};