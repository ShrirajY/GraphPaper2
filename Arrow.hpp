// Line.hpp
#pragma once
#include <windows.h>
#include "Globals.hpp"

class Arrow
{
public:
    Arrow(POINT start, POINT end) : start_(start), end_(end), next_(nullptr), color(currColor) {}
    POINT start_, end_;
    COLORREF color = RGB(255, 0, 0); // Default color red
    Arrow *next_;
};

class ArrowList
{
public:
    ArrowList() : head_(nullptr) {}
    ~ArrowList() { Clear(); }

    void Add(POINT &start, POINT &end, HDC hdc)
    {
        if (!hdc)
            return; // Ensure hdc is valid
        // Convert logical coordinates to device coordinates
        SetMapMode(hdc, MM_ANISOTROPIC);
        SetWindowExtEx(hdc, 800, 600, NULL);
        SetViewportExtEx(hdc, 800, -600, NULL);        // Invert Y
        SetViewportOrgEx(hdc, 800 / 2, 600 / 2, NULL); // Center origin

        DPtoLP(hdc, &start, 1);
        DPtoLP(hdc, &end, 1);
        Arrow *node = new Arrow(start, end);
        node->next_ = head_;
        head_ = node;
    }

    void DrawAll(HDC hdc) const
    {
        for (Arrow *node = head_; node; node = node->next_)
        {
            // Example: width 3, color red
            HPEN hPen = CreatePen(PS_SOLID, 3, node->color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            MoveToEx(hdc, node->start_.x, node->start_.y, NULL);
            LineTo(hdc, node->end_.x, node->end_.y);
            this->DrawArrowHead(hdc, node->start_, node->end_, 15, 35.0); // Draw arrowhead
            // Restore old pen and delete the created pen
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
    }

    void Clear()
    {
        while (head_)
        {
            Arrow *next = head_->next_;
            delete head_;
            head_ = next;
        }
    }

    // Draws an arrowhead at the end of the line from start to end
    static void DrawArrowHead(HDC hdc, POINT start, POINT end, int size = 15, double angleDeg = 20.0)
    {
        double angle = atan2(end.y - start.y, end.x - start.x);
        double angle1 = angle + angleDeg * 3.14159265 / 180.0;
        double angle2 = angle - angleDeg * 3.14159265 / 180.0;

        POINT tip1, tip2;
        tip1.x = static_cast<LONG>(end.x - size * cos(angle1));
        tip1.y = static_cast<LONG>(end.y - size * sin(angle1));
        tip2.x = static_cast<LONG>(end.x - size * cos(angle2));
        tip2.y = static_cast<LONG>(end.y - size * sin(angle2));

        MoveToEx(hdc, end.x, end.y, NULL);
        LineTo(hdc, tip1.x, tip1.y);

        MoveToEx(hdc, end.x, end.y, NULL);
        LineTo(hdc, tip2.x, tip2.y);
    }

private:
    Arrow *head_;
};

ArrowList g_arrows;
POINT g_tempStart;
bool g_drawing = false;