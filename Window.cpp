#include <windows.h>
#include <cmath>
#include "Axis.hpp"
#include "ShapeDrawer.hpp"
#include "Globals.hpp"
#include "Circle.hpp"
#include "Line.hpp"
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

    static AxisDrawer *axisDrawer = nullptr;
    switch (msg)
    {
    case WM_CREATE:
    {
        HDC hdc = GetDC(hwnd);
        axisDrawer = new AxisDrawer(hdc, GraphWidth, GraphHeight);
        ReleaseDC(hwnd, hdc);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Set up the coordinate system
        if (axisDrawer)
        {
            axisDrawer->Draw();
        }

        if(!axisDrawer)
        {
            MessageBox(hwnd, TEXT("AxisDrawer not initialized!"), TEXT("Error"), MB_OK | MB_ICONERROR);
            axisDrawer = new AxisDrawer(hdc, GraphWidth, GraphHeight);
            axisDrawer->Draw();
        }

        // Draw Sample Circle
        ShapeDrawer shapeDrawer(hdc);
        Circle circle(hdc, 4, 3, 2, RGB(255, 0, 0));
        shapeDrawer.DrawCircle(&circle);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}