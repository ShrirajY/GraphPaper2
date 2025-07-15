#pragma once
#include <windows.h>
#include <iostream>
#include <cmath>
#include "Globals.hpp" // Assuming this header defines the constants like GraphWidth, GraphHeight, etc.
#include "Circle.hpp"
#include "Line.hpp"
#include "Ellipse.hpp"
#include "Parabola.hpp"
#include "Parser/parser.h" // Assuming this is where the Parsefile function is defined

void WriteLogToAction();

class ShapeDrawer
{
public:
    ShapeDrawer(HDC hdc);

    void DrawPoint(float x, float y, COLORREF color, int size = DefaultPointSize) const;
    void DrawLine(Line *line) const;
    void DrawRectangle(float x1, float y1, float x2, float y2, COLORREF color) const;
    void DrawCircle(Circle *circle) const;
    void DrawEllipse(Ellipse_ *ellipse) const;
    void DrawParabola(Parabola *parabola) const;

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

void ShapeDrawer::DrawLine(Line *line) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, line->color_);
    HGDIOBJ oldPen = SelectObject(hdc_, hPen);

    int startX = static_cast<int>(line->x1_ * Scale);
    int startY = static_cast<int>(line->y1_ * Scale);
    int endX = static_cast<int>(line->x2_ * Scale);
    int endY = static_cast<int>(line->y2_ * Scale);

    // DrawPoint(line->x1_, line->y1_, line->color_, LinePointSize);
    // DrawPoint(line->x2_, line->y2_, line->color_, LinePointSize);

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

void ShapeDrawer::DrawCircle(Circle *circle) const
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

void ShapeDrawer::DrawEllipse(Ellipse_ *ellipse) const
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

void ShapeDrawer::DrawParabola(Parabola *parabola) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, parabola->getColor());
    if (!hPen)
    {
        // Handle error creating pen
        return;
    }
    HGDIOBJ oldPen = SelectObject(hdc_, hPen);
    if (!oldPen)
    {
        // Handle error selecting pen, delete newly created pen
        DeleteObject(hPen);
        return;
    }

    const int segments = 100; // Number of line segments to approximate the curve
    POINT pts[segments];      // Array to store the points

    // Get parabola parameters
    float vx = parabola->getVertexX();
    float vy = parabola->getVertexY();
    float x1 = parabola->getX1();
    float y1 = parabola->getY1();
    float x2 = parabola->getX2();
    float y2 = parabola->getY2();

    // --- Step 1: Translate all points so the vertex is at the origin (0,0) ---
    // P1_prime, P2_prime are the endpoints relative to the vertex
    float px1 = x1 - vx;
    float py1 = y1 - vy;
    float px2 = x2 - vx;
    float py2 = y2 - vy;

    // --- Step 2: Determine the axis of symmetry's orientation ---
    // We assume the axis of symmetry passes through the vertex (now origin)
    // and the midpoint of the chord connecting P1_prime and P2_prime.
    float midX_prime = (px1 + px2) / 2.0f;
    float midY_prime = (py1 + py2) / 2.0f;

    // Calculate the angle of this axis relative to the positive X-axis
    // atan2 is robust for all quadrants.
    float axisAngle = std::atan2(midY_prime, midX_prime);

    // Calculate the rotation angle needed to align this axis with the positive Y-axis.
    // If axis is along positive X (angle 0), rotate by PI/2.
    // If axis is along positive Y (angle PI/2), rotate by 0.
    float rotationAngle = (static_cast<float>(M_PI) / 2.0f) - axisAngle;

    // Pre-calculate sin and cos for efficiency
    float cosR = std::cos(rotationAngle);
    float sinR = std::sin(rotationAngle);

    // Helper lambda for rotating a point (x, y) by 'rotationAngle'
    auto rotatePoint = [&](float x, float y, float &xr, float &yr)
    {
        xr = x * cosR - y * sinR;
        yr = x * sinR + y * cosR;
    };

    // Rotate the transformed endpoints (P1_prime, P2_prime)
    // Now, in this 'rotated' coordinate system, the parabola's axis is the Y-axis,
    // and its vertex is still at the origin (0,0).
    // Its equation will be of the form: y' = a * x'^2
    float rx1, ry1; // Rotated P1_prime
    float rx2, ry2; // Rotated P2_prime
    rotatePoint(px1, py1, rx1, ry1);
    rotatePoint(px2, py2, rx2, ry2);

    // --- Step 3: Calculate the 'a' coefficient ---
    // We use one of the rotated endpoints to solve for 'a'.
    // If rx1 is effectively zero (meaning P1_prime was on the rotated Y-axis),
    // we might have a division by zero or a degenerate case. Try rx2.
    float a;
    bool a_calculated = false;
    const float EPSILON = 1e-6f; // Small value for float comparisons

    if (std::abs(rx1) > EPSILON)
    {
        a = ry1 / (rx1 * rx1);
        a_calculated = true;
    }
    else if (std::abs(rx2) > EPSILON)
    {
        // If rx1 is too small, try using rx2.
        // This is especially important if (px1, py1) was exactly on the axis of symmetry.
        a = ry2 / (rx2 * rx2);
        a_calculated = true;
    }

    if (!a_calculated || !std::isfinite(a))
    {
        // Cannot determine 'a' or 'a' is infinite/NaN (degenerate parabola, e.g., vertical line or point)
        // Cleanup GDI objects and return.
        SelectObject(hdc_, oldPen);
        DeleteObject(hPen);
        std::cerr << "Warning: Degenerate parabola 'a' coefficient detected." << std::endl; // Debug
        return;
    }

    // --- Step 4: Determine the range of x-values for generating the parabola segment ---
    // The segment will be drawn between the rotated x-coordinates of the two endpoints.
    float minX_rotated_segment = std::min(rx1, rx2);
    float maxX_rotated_segment = std::max(rx1, rx2);

    // Ensure a valid range for drawing
    if (segments <= 1 || std::abs(maxX_rotated_segment - minX_rotated_segment) < EPSILON)
    {
        // Endpoints are too close or identical in the rotated X-axis.
        // This would result in a single point or a very short, invisible line.
        SelectObject(hdc_, oldPen);
        DeleteObject(hPen);
        // Optionally, draw a single point at the vertex or midpoint if appropriate.
        std::cerr << "Warning: Parabola segment too small or degenerate for drawing." << std::endl; // Debug
        return;
    }

    // --- Step 5: Generate points for the parabola segment ---
    // Loop through the desired number of segments
    for (int i = 0; i < segments; ++i)
    {
        // Calculate x-coordinate in the rotated system for the current segment point
        float x_rotated_seg = minX_rotated_segment + (maxX_rotated_segment - minX_rotated_segment) * i / (segments - 1);

        // Calculate corresponding y-coordinate in the rotated system using y' = a * x'^2
        float y_rotated_seg = a * x_rotated_seg * x_rotated_seg;

        // --- Step 6: Rotate the point back to the original orientation ---
        // (Vertex is still at the origin for this step)
        float xr_final, yr_final;
        // Rotation back by -rotationAngle.
        // cos(-theta) = cos(theta)
        // sin(-theta) = -sin(theta)
        float cosR_inv = cosR;
        float sinR_inv = -sinR;

        xr_final = x_rotated_seg * cosR_inv - y_rotated_seg * sinR_inv;
        yr_final = x_rotated_seg * sinR_inv + y_rotated_seg * cosR_inv;

        // --- Step 7: Translate the point back to original screen coordinates ---
        float finalX_screen = vx + xr_final;
        float finalY_screen = vy + yr_final;

        // Apply scaling and store in the POINT array for GDI
        pts[i].x = static_cast<LONG>(finalX_screen * Scale);
        pts[i].y = static_cast<LONG>(finalY_screen * Scale);
    }

    // --- Step 8: Draw the parabola segment using Polyline ---
    Polyline(hdc_, pts, segments);

    // Debugging rectangle: This draws a small rectangle at fixed screen coordinates.
    // It's useful to test if GDI drawing is working at all. Remove for final code.
    // Rectangle(hdc_, 5, 5, 10, 15);

    // --- Step 9: Cleanup GDI objects ---
    SelectObject(hdc_, oldPen); // Restore original pen to the DC
    DeleteObject(hPen);         // Delete the pen we created
}

void Parsefile(HWND hwnd, char *filename)
{
    int response = MessageBox(hwnd, TEXT("Do you want to keep previous drawing?"), TEXT("Confirm"), MB_YESNO | MB_ICONQUESTION);
    if (response == IDNO)
    {
        lineList.clear();
        CircleList.clear();
        ellipseList.clear();
        parabolaList.clear();
        actionLog.clear();
    }
    parse(filename);                  // Call the parse function with the filename
    WriteLogToAction();               // Write the parsed shapes to the action log
    InvalidateRect(hwnd, NULL, TRUE); // Redraw the window to reflect changes
}

void LineToLog(const Line &line)
{
    std::string action = "LINE: " + std::to_string(line.getX1()) + ", " + std::to_string(line.getY1()) + ", " +
                         std::to_string(line.getX2()) + ", " + std::to_string(line.getY2()) + ", (" +
                         std::to_string(GetRValue(line.getColor())) + ", " +
                         std::to_string(GetGValue(line.getColor())) + ", " +
                         std::to_string(GetBValue(line.getColor())) + ")";
    actionLog.push_back(action);
}

void CircleToLog(const Circle &circle)
{
    std::string action = "CIRCLE: " + std::to_string(circle.GetCenterX()) + ", " + std::to_string(circle.GetCenterY()) + ", " +
                         std::to_string(circle.GetRadius()) + ", (" +
                         std::to_string(GetRValue(circle.GetColor())) + ", " +
                         std::to_string(GetGValue(circle.GetColor())) + ", " +
                         std::to_string(GetBValue(circle.GetColor())) + ")";
    actionLog.push_back(action);
}

void EllipseToLog(const Ellipse_ &ellipse)
{
    std::string action = "ELLIPSE: " + std::to_string(ellipse.getCenterX()) + ", " + std::to_string(ellipse.getCenterY()) + ", " +
                         std::to_string(ellipse.getA()) + ", " + std::to_string(ellipse.getB()) + ", " +
                         std::to_string(ellipse.getAngle()) + ", (" +
                         std::to_string(GetRValue(ellipse.getColor())) + ", " +
                         std::to_string(GetGValue(ellipse.getColor())) + ", " +
                         std::to_string(GetBValue(ellipse.getColor())) + ")";
    actionLog.push_back(action);
}

void WriteLogToAction()
{
    for (const auto &line : lineList)
    {
        LineToLog(line);
    }

    for (const auto &circle : CircleList)
    {
        CircleToLog(circle);
    }

    for (const auto &ellipse : ellipseList)
    {
        EllipseToLog(ellipse);
    }
}
