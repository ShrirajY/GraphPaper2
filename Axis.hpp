#ifndef AXIS_HPP
#define AXIS_HPP

#pragma once
#include <windows.h>
#include "Globals.hpp"
class AxisDrawer
{
public:
    AxisDrawer(HDC hdc, int width, int height);

    void Draw();

private:
    void SetupMapping();
    void DrawMainAxes();
    void DrawTicks();
    void DrawGrid();

    HDC hdc_;
    int width_;
    int height_;
    int tickSpacing_;
    int tickSize_;
};

AxisDrawer::AxisDrawer(HDC hdc, int width, int height)
    : hdc_(hdc), width_(width), height_(height), tickSpacing_(tickSpacing), tickSize_(tickSize) {}

void AxisDrawer::Draw()
{
    SetupMapping();
    DrawMainAxes();
    DrawTicks();
    DrawGrid();
}

void AxisDrawer::SetupMapping()
{
    SetMapMode(hdc_, MM_ANISOTROPIC);
    SetWindowExtEx(hdc_, width_, height_, NULL);
    SetViewportExtEx(hdc_, width_, -height_, NULL);        // Invert Y axis
    SetViewportOrgEx(hdc_, width_ / 2, height_ / 2, NULL); // Origin center
}

void AxisDrawer::DrawMainAxes()
{
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(10, 10, 10));
    HPEN hOldPen = (HPEN)SelectObject(hdc_, hPen);

    // X axis
    MoveToEx(hdc_, -width_ / 2, 0, NULL);
    LineTo(hdc_, width_ / 2, 0);

    // Y axis
    MoveToEx(hdc_, 0, -height_ / 2, NULL);
    LineTo(hdc_, 0, height_ / 2);

    SelectObject(hdc_, hOldPen);
    DeleteObject(hPen);
}

void AxisDrawer::DrawTicks()
{
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(10, 10, 10));
    HPEN hOldPen = (HPEN)SelectObject(hdc_, hPen);

    // X-axis ticks
    for (int x = -width_ / 2; x <= width_ / 2; x += tickSpacing_)
    {
        MoveToEx(hdc_, x, -tickSize_, NULL);
        LineTo(hdc_, x, tickSize_);
    }

    // Y-axis ticks
    for (int y = -height_ / 2; y <= height_ / 2; y += tickSpacing_)
    {
        MoveToEx(hdc_, -tickSize_, y, NULL);
        LineTo(hdc_, tickSize_, y);
    }

    SelectObject(hdc_, hOldPen);
    DeleteObject(hPen);
}

void AxisDrawer::DrawGrid()
{
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(210, 210, 210));
    HPEN hOldPen = (HPEN)SelectObject(hdc_, hPen);

    // Horizontal grid lines
    for (int y = -height_ / 2; y <= height_ / 2; y += tickSpacing_)
    {
        MoveToEx(hdc_, -width_ / 2, y, NULL);
        LineTo(hdc_, width_ / 2, y);
    }

    // Vertical grid lines
    for (int x = -width_ / 2; x <= width_ / 2; x += tickSpacing_)
    {
        MoveToEx(hdc_, x, -height_ / 2, NULL);
        LineTo(hdc_, x, height_ / 2);
    }

    SelectObject(hdc_, hOldPen);
    DeleteObject(hPen);
}

#endif // AXIS_HPP