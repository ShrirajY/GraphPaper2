// ShapeManager.hpp
#pragma once
#include <list>
#include <cmath>
#include <string>
#include <windows.h>
#include "Line.hpp"
#include "Circle.hpp"
#include "Ellipse.hpp"
#include "Globals.hpp"

std::vector<COLORREF> highlightColors = { 
    // Highlighting colors for selected shapes
    RGB(255, 255, 0), // Yellow
    RGB(0, 255, 0),   // Green
    RGB(0, 0, 255),   // Blue
    RGB(255, 0, 0),   // Red
    RGB(255, 128, 0), // Orange
 };

int CurrentHighlightColorIndex = 0; // Index for cycling through highlight colors

class ShapeManager
{
public:
    // Hit test: returns "Line", "Circle", "Ellipse", or "None"
    std::string HitTest(float x, float y, float tol = 5.0f) const
    {
        // Check lines
        for (auto &line : lineList)
        {
            if (IsPointOnLineSegment(x, y, line.x1_, line.y1_, line.x2_, line.y2_, 0.5))
            {
                OldColor = line.getColor(); // Store the old color
                // line.setColor(RGB(255, 255, 0));   // Change color to yellow for hit
                SelectedLine = &line; // Store the selected line
                SelectedShape = 1;   // Set selected shape to Line
                SetTimer(hMain, 1, 100, NULL); // Set a timer for highlighting
                InvalidateRect(hMain, NULL, TRUE); // Redraw the window
                return "Line";
            }
        }
        // Check circles
        for (auto &circle : CircleList)
        {
            if (IsPointOnCircle(x, y, circle.centerX_, circle.centerY_, circle.radius_, tol))
            {
                OldColor = circle.GetColor(); // Store the old color
                SelectedCircle = &circle; // Store the selected circle
                SelectedShape = 2;       // Set selected shape to Circle
                SetTimer(hMain, 1, 100, NULL); // Set a timer for highlighting
                InvalidateRect(hMain, NULL, TRUE); // Redraw the window
                return "Circle";
            }
        }
        // Check ellipses
        for (auto &ellipse : ellipseList)
        {
            if (IsPointOnEllipse(x, y, ellipse.centerX_, ellipse.centerY_, ellipse.a_, ellipse.b_, tol, ellipse.angle_))
            {
                OldColor = ellipse.getColor(); // Store the old color
                SelectedEllipse = &ellipse; // Store the selected ellipse
                SelectedShape = 3;       // Set selected shape to Ellipse
                SetTimer(hMain, 1, 100, NULL); // Set a timer for highlighting
                InvalidateRect(hMain, NULL, TRUE); // Redraw the window
                return "Ellipse";
            }
        }
        
        return "None";
    }

private:
    // Helper functions
    static bool IsPointOnLineSegment(double x, double y, double x1, double y1, double x2, double y2, double tol)
    {
        double dx = x2 - x1, dy = y2 - y1;
        double len_sq = dx * dx + dy * dy;
        if (len_sq == 0.0)
            return (std::hypot(x - x1, y - y1) <= tol);
        double t = ((x - x1) * dx + (y - y1) * dy) / len_sq;
        if (t < 0.0 || t > 1.0)
            return false;
        double proj_x = x1 + t * dx;
        double proj_y = y1 + t * dy;
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

void SetTimerForHighlighting()
{
    // Set a timer for highlighting the selected shape
    if(SelectedShape == 1) // Line
    {
        SelectedLine->setColor(highlightColors[CurrentHighlightColorIndex]);
    }
    else if(SelectedShape == 2) // Circle
    {
        SelectedCircle->SetColor(highlightColors[CurrentHighlightColorIndex]);
    }
    else if(SelectedShape == 3) // Ellipse
    {
        SelectedEllipse->setColor(highlightColors[CurrentHighlightColorIndex]);
    }
    CurrentHighlightColorIndex = (CurrentHighlightColorIndex + 1) % highlightColors.size();
    SetTimer(hMain, 1, 1000, NULL);
    InvalidateRect(hMain, NULL, TRUE); // Redraw the window
}

void ResetHighlighting()
{
    if(SelectedShape == 1) // Line
    {
        SelectedLine->setColor(OldColor);
    }
    else if(SelectedShape == 2) // Circle
    {
        SelectedCircle->SetColor(OldColor);
    }
    else if(SelectedShape == 3) // Ellipse
    {
        SelectedEllipse->setColor(OldColor);
    }
    InvalidateRect(hMain, NULL, TRUE); // Redraw the window
}