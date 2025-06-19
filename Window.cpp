#include <windows.h>
#include <cmath>
#include "Axis.hpp"
#include "ShapeDrawer.hpp"
#include "Globals.hpp"
#include "Circle.hpp"
#include "Line.hpp"

#include "DrawGB/DrawCircle.hpp"
#include "DrawGB/DrawLine.hpp"
#include "DrawGB/DrawEllipse.hpp"
#include "DrawGB/DrawDropDownBox.hpp"

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

    switch (msg)
    {
    case WM_CREATE:
    {
        HDC hdc = GetDC(hwnd);
        ReleaseDC(hwnd, hdc);

        GroupBoxCircle *gbCircle = new GroupBoxCircle();
        gbCircle->DrawGroupBoxCircle(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        GroupBoxLine *gbLine = new GroupBoxLine();
        gbLine->DrawGroupBoxLine(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        GroupBoxEllipse *gbEllipse = new GroupBoxEllipse();
        gbEllipse->DrawGroupBoxEllipse(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

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
        break;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}