#include <windows.h>
#include <cmath>
#include <windowsx.h>
#include <tchar.h>
#include "Axis.hpp"
#include "ShapeDrawer.hpp"
#include "Globals.hpp"
#include "Circle.hpp"
#include "Line.hpp"
#include "Ellipse.hpp"
#include "Arrow.hpp"
#include "HitTest.hpp"

#include "DrawGB/DrawCircle.hpp"
#include "DrawGB/DrawLine.hpp"
#include "DrawGB/DrawEllipse.hpp"
#include "DrawGB/DrawDropDownBox.hpp"

#include "ShowInfo/ShowShapesInfo.hpp"

#include "Parser/parser.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("CubeWinClass");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    RECT desired = {0, 0, 800, 600};
    AdjustWindowRect(&desired, WS_OVERLAPPEDWINDOW, FALSE);

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindow(TEXT("CubeWinClass"), TEXT("3D Cube in Win32 (No OpenGL)"),
                             WS_OVERLAPPEDWINDOW,
                             0, 0, screenWidth, screenHeight,
                             NULL, NULL, hInstance, NULL);
    hMain = hwnd; // Store the main window handle
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ShapeManager *shapeManager = new ShapeManager();
    static GroupBoxEllipse *gbEllipse = nullptr;
    static GroupBoxLine *gbLine = nullptr;
    static GroupBoxCircle *gbCircle = nullptr;
    static ShowShapesInfo *showShapesInfo = nullptr;
    switch (msg)
    {
    case WM_CREATE:
    {
        HDC hdc = GetDC(hwnd);
        ReleaseDC(hwnd, hdc);

        gbCircle = new GroupBoxCircle();
        gbCircle->DrawGroupBoxCircle(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        gbLine = new GroupBoxLine();
        gbLine->DrawGroupBoxLine(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        gbEllipse = new GroupBoxEllipse();
        gbEllipse->DrawGroupBoxEllipse(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        showShapesInfo = new ShowShapesInfo();
        showShapesInfo->DrawShowGroupBox(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        ActiveGroupBox = 0; // Default to Line group box

        ShowWindow(hDGBLine, SW_SHOW);
        ShowWindow(hDGBCircle, SW_HIDE);
        ShowWindow(hDGBEllipse, SW_HIDE);
        DrawDropDownBox(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        char filename[] = "shapes.txt";
        parse(filename);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 230));
        // SelectObject(hdc, hBrush);

        // DeleteObject(hBrush);
        // Set up the coordinate system
        AxisDrawer axisDrawer(hdc, GraphWidth, GraphHeight);
        axisDrawer.Draw();

        // Draw Sample Circle
        ShapeDrawer shapeDrawer(hdc);
        for (Circle &circle : CircleList)
        {
            shapeDrawer.DrawCircle(&circle);
        }
        for (Line &line : lineList)
        {
            shapeDrawer.DrawLine(&line);
        }
        for (Ellipse_ &ellipse : ellipseList)
        {
            shapeDrawer.DrawEllipse(&ellipse);
        }
        // FillBox(hDGBCircle, RGB(100, 100, 100));
        // FillBox(hDGBLine, RGB(100, 100, 100));

        if (ActiveGroupBox == 0)
        {
            FillBox(hDGBLine, grayCColor);
        }
        else if (ActiveGroupBox == 1)
        {
            FillBox(hDGBCircle, grayCColor);
        }
        else if (ActiveGroupBox == 2)
        {
            FillBox(hDGBEllipse, grayCColor);
        }

        if (hShowInfo)
        {
            FillBox(hShowInfo, candyCColor);
        }

        g_arrows.DrawAll(hdc); // Draw all arrows
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
        SelectObject(hdc, hBrush);

        DeleteObject(hBrush);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == 2001 && HIWORD(wParam) == CBN_SELCHANGE)
        {
            int selectedIndex = SendMessage(hDropDown, CB_GETCURSEL, 0, 0);
            ActiveGroupBox = selectedIndex; // Update active group box based on selection
            ShowWindow(hDGBLine, selectedIndex == 0 ? SW_SHOW : SW_HIDE);
            ShowWindow(hDGBCircle, selectedIndex == 1 ? SW_SHOW : SW_HIDE);
            ShowWindow(hDGBEllipse, selectedIndex == 2 ? SW_SHOW : SW_HIDE);
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
        }

        if (LOWORD(wParam) == 2002 && HIWORD(wParam) == CBN_SELCHANGE)
        {
            int selectedIndex = SendMessage(hDropDownFeature, CB_GETCURSEL, 0, 0);
            ActiveFeature = selectedIndex;    // Update active feature based on selection
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
        }

        if (HIWORD(wParam) == EN_SETFOCUS)
        {
            HWND hNewFocus = (HWND)lParam;
            if (hPrevEdit && hPrevEdit != hNewFocus)
            {
                EnableWindow(hPrevEdit, FALSE);
                SetFocus(hMain);
                SetFocus(hNewFocus);
            }
            hPrevEdit = hNewFocus;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CTLCOLORSTATIC:
    {
        HWND hCtrl = (HWND)lParam;
        if (hCtrl == hDGBCircle || hCtrl == hDGBLine || hCtrl == hDGBEllipse)
        {
            return ColorBG::ColorBGOpaque((HDC)wParam, grayCColor, redCColor);
        }
        if (hCtrl == hShowInfo)
        {
            return ColorBG::ColorBGOpaque((HDC)wParam, grayCColor, yellowCColor);
        }
    }
    case WM_CTLCOLOREDIT:
    {
        HDC hdcEdit = (HDC)wParam;
        SetBkColor(hdcEdit, RGB(205, 193, 255));
        SetTextColor(hdcEdit, RGB(0, 0, 0)); // Invert text color for visibility
        return (LRESULT)hTransparentBrush;
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (SelectedShape != 0)
        {
            ResetHighlighting();
            KillTimer(hMain, 1); // Stop the timer if a shape is selected
            SelectedShape = 0;   // Reset selected shape
        }
        if (ActiveFeature == 0)
        {
            // Drawing arrows
            g_tempStart.x = GET_X_LPARAM(lParam);
            g_tempStart.y = GET_Y_LPARAM(lParam);
            g_drawing = true;
        }
        else if (ActiveFeature == 1)
        {
            HDC hdc = GetDC(hwnd); // Get the DC once

            SetMapMode(hdc, MM_ANISOTROPIC);
            SetWindowExtEx(hdc, GraphWidth, GraphHeight, NULL);
            SetViewportExtEx(hdc, GraphWidth, -GraphHeight, NULL);        // Invert Y axis
            SetViewportOrgEx(hdc, GraphWidth / 2, GraphHeight / 2, NULL); // Origin center

            POINT point;
            point.x = GET_X_LPARAM(lParam);
            point.y = GET_Y_LPARAM(lParam);
            DPtoLP(hdc, &point, 1); // Now this will work as expected
            shapeManager->HitTest(point.x / 25, point.y / 25, 1.0f);
        }
        else if (ActiveFeature == 2)
        {
            HDC hdc = GetDC(hwnd); // Get the DC once

            SetMapMode(hdc, MM_ANISOTROPIC);
            SetWindowExtEx(hdc, GraphWidth, GraphHeight, NULL);
            SetViewportExtEx(hdc, GraphWidth, -GraphHeight, NULL);        // Invert Y axis
            SetViewportOrgEx(hdc, GraphWidth / 2, GraphHeight / 2, NULL); // Origin center

            POINT point;
            point.x = GET_X_LPARAM(lParam);
            point.y = GET_Y_LPARAM(lParam);
            DPtoLP(hdc, &point, 1); // Now this will work as expected
            COLORREF ReverseColor = RGB(GetBValue(currColor),
                                        GetGValue(currColor),
                                        GetRValue(currColor));
            FloodFillCustom(hdc, point.x, point.y, ReverseColor, BackgroundColors);
        }

        

        // 
        // MessageBox(hwnd, buffer, TEXT("Coordinates"), MB_OK);
        // MessageBox(NULL, TEXT("Hit Test"), TEXT("Hit Test"), MB_OK);
        Arrow::DisableEditIfInBounds(hMain);
        break;
    }

    case WM_LBUTTONUP:
    {
        if (g_drawing)
        {
            POINT end;
            end.x = GET_X_LPARAM(lParam);
            end.y = GET_Y_LPARAM(lParam);
            g_arrows.Add(g_tempStart, end, GetDC(hwnd));
            g_drawing = false;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    }

    case WM_RBUTTONDOWN:
    {
        if(ActiveFeature == 0)
        {
            g_drawing = false; // Stop drawing arrows on right-click
            ActiveFeature = -1;
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
        }
        else if(ActiveFeature == -1)
        {
            g_drawing = true; // Resume drawing arrows on right-click
            ActiveFeature = 0;
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
        }
        break;
    }

    case WM_TIMER:
    {
        if (SelectedShape != 0)
        {
            SetTimerForHighlighting();
        }
        break;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}