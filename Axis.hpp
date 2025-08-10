#ifndef AXIS_HPP
#define AXIS_HPP

#pragma once
#include <windows.h>
#include "Globals.hpp"

#include <queue>
#include <set>
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

#include <vector>
#include <queue>
#include <set>

void FloodFillCustom(HDC hdc, int startX, int startY, COLORREF fillColor, std::set<COLORREF> stopColors)
{
    const int maxX = 400;
    const int maxY = 300;

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 2 * maxX;    // 800
    bmi.bmiHeader.biHeight = 2 * maxY;  // -600 (top-down)
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void *bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!hBmp || !bits)
        return;

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBmp);

    // Copy the area to memory (centered at (0,0))
    BitBlt(memDC, 0, 0, 2 * maxX, 2 * maxY, hdc, -maxX, -maxY, SRCCOPY);

    auto getPixel = [&](int x, int y) -> COLORREF
    {
        int bufX = x + maxX;         // -400..399 -> 0..799
        int bufY = maxY - 1 - y;     // -300..299 -> 299..0 (Y up)
        if (bufX < 0 || bufX >= 2 * maxX || bufY < 0 || bufY >= 2 * maxY)
            return 0xFFFFFFFF;
        return ((COLORREF *)bits)[bufY * (2 * maxX) + bufX];
    };
    auto setPixel = [&](int x, int y, COLORREF c)
    {
        int bufX = x + maxX;
        int bufY = maxY - 1 - y;
        if (bufX < 0 || bufX >= 2 * maxX || bufY < 0 || bufY >= 2 * maxY)
            return;
        ((COLORREF *)bits)[bufY * (2 * maxX) + bufX] = c;
    };

    COLORREF startColor = getPixel(startX, startY);
    if (startColor == fillColor)
    {
        SelectObject(memDC, oldBmp);
        DeleteObject(hBmp);
        DeleteDC(memDC);
        return;
    }

    std::queue<POINT> q;
    q.push({startX, startY});

    while (!q.empty())
    {
        POINT p = q.front();
        q.pop();

        COLORREF currentColor = getPixel(p.x, p.y);

        if (!stopColors.count(currentColor) || currentColor == fillColor)
            continue;

        setPixel(p.x, p.y, fillColor);

        if (p.x > -maxX)
            q.push({p.x - 1, p.y});
        if (p.y > -maxY)
            q.push({p.x, p.y - 1});
        if (p.x < maxX - 1)
            q.push({p.x + 1, p.y});
        if (p.y < maxY - 1)
            q.push({p.x, p.y + 1});
    }

    // Copy memory back to screen, aligning DIB's (0,0) to logical (-400,-300)
    BitBlt(hdc, -maxX, -maxY, 2 * maxX, 2 * maxY, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
}

void AddFloodFillPoint(HWND hwnd, std::pair<POINT, COLORREF> floodFillPoint)
{
    int startX = floodFillPoint.first.x;
    int startY = floodFillPoint.first.y;
    COLORREF fillColor = floodFillPoint.second;
    POINT pt = {startX, startY};
    std::string action = "FILLCOLOR: " + std::to_string(startX) + ", " + std::to_string(startY) + ",(" +
                         std::to_string(GetBValue(fillColor)) + ", " +
                         std::to_string(GetGValue(fillColor)) + ", " +
                         std::to_string(GetRValue(fillColor)) + ")";
    CodeAction(action);
    floodFillPointsList.push_back({pt, fillColor});
}

std::set<COLORREF> BackgroundColors = {
    RGB(255, 255, 255), // background
    RGB(10, 10, 10),    // axis
    RGB(210, 210, 210)  // grid
};

#endif // AXIS_HPP