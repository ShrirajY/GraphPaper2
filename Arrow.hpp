// Line.hpp
#pragma once
#include <windows.h>
#include "Globals.hpp"

class Arrow
{
public:
    Arrow(POINT start, POINT end) : start_(start), end_(end), next_(nullptr), color(currColor)
    {
    }
    POINT start_, end_;
    COLORREF color = RGB(255, 0, 0); // Default color red
    Arrow *next_;
    HWND hEdit = nullptr; // Handle to the edit control for this arrow

    static void DisableEditIfInBounds(HWND hMain)
    {
        HWND hWndFocus = GetFocus();

        if (hWndFocus != NULL && IsChild(hMain, hWndFocus))
        {
            // Check if the focused control is an Edit control
            TCHAR className[256];
            GetClassName(hWndFocus, className, sizeof(className) / sizeof(TCHAR));

            if (_tcscmp(className, TEXT("Edit")) == 0) // Confirm it's an Edit control
            {
                // Get the position of the control relative to the main window
                RECT rcControl;
                GetWindowRect(hWndFocus, &rcControl);
                ScreenToClient(hMain, (LPPOINT)&rcControl.left);
                ScreenToClient(hMain, (LPPOINT)&rcControl.right);

                int width = rcControl.right - rcControl.left;
                int height = rcControl.bottom - rcControl.top;

                // Check if it's within 800x600 bounds
                if (rcControl.left >= 0 && rcControl.top >= 0 &&
                    rcControl.right <= 800 && rcControl.bottom <= 600)
                {
                    // Remove focus
                    SetFocus(hMain);
                    EnableWindow(hWndFocus, FALSE);
                }
            }
        }
    }
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

        const int xMin = -GraphWidth / 2;
        const int xMax = GraphWidth / 2;
        const int yMin = -GraphHeight / 2;
        const int yMax = GraphHeight / 2;

        if (start.x < xMin || start.x > xMax ||
            end.x < xMin || end.x > xMax ||
            start.y < yMin || start.y > yMax ||
            end.y < yMin || end.y > yMax)
        {
            return; // Skip drawing this arrow
        }

        Arrow *node = new Arrow(start, end);
        POINT deviceEnd = end;
        LPtoDP(hdc, &deviceEnd, 1);
        node->hEdit = CreateWindowEx(
            0, TEXT("EDIT"), TEXT(""),
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            deviceEnd.x + 5, deviceEnd.y + 5, 80, 20,
            hMain, NULL, (HINSTANCE)GetWindowLongPtr(hMain, GWLP_HINSTANCE), NULL);
        node->next_ = head_;
        head_ = node;
    }

    void DrawAll(HDC hdc) const
    {
        const int xMin = -GraphWidth / 2;
        const int xMax = GraphWidth / 2;
        const int yMin = -GraphHeight / 2;
        const int yMax = GraphHeight / 2;

        for (Arrow *node = head_; node; node = node->next_)
        {
            // Check if both start and end are within the logical window
            if (node->start_.x < xMin || node->start_.x > xMax ||
                node->end_.x < xMin || node->end_.x > xMax ||
                node->start_.y < yMin || node->start_.y > yMax ||
                node->end_.y < yMin || node->end_.y > yMax)
            {
                continue; // Skip drawing this arrow
            }

            HPEN hPen = CreatePen(PS_SOLID, 3, node->color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            MoveToEx(hdc, node->start_.x, node->start_.y, NULL);
            LineTo(hdc, node->end_.x, node->end_.y);
            DrawArrowHead(hdc, node->start_, node->end_, 15, 35.0);

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