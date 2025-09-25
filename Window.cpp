#include <windows.h>
#include <cmath>
#include <windowsx.h>
#include <tchar.h>
#include <mmsystem.h>  // For PlaySound
#pragma comment(lib, "winmm.lib")


#include "Axis.hpp"
#include "ShapeDrawer.hpp"
#include "Globals.hpp"
#include "Circle.hpp"
#include "Line.hpp"
#include "Ellipse.hpp"
#include "Parabola.hpp"
#include "Arrow.hpp"
#include "HitTest.hpp"
#include "Menu.hpp"

#include "DrawGB/DrawCircle.hpp"
#include "DrawGB/DrawLine.hpp"
#include "DrawGB/DrawEllipse.hpp"
#include "DrawGB/DrawDropDownBox.hpp"
#include "DrawGB/DrawParabola.hpp"
#include "DrawGB/DrawImage.hpp"

#include "ImageLoading/Image.hpp"
#include "About/About.hpp"

#include "ShowInfo/ShowShapesInfo.hpp"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("Graph Paper");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Dark blue background
    wc.hIcon = (HICON)LoadImage(NULL, TEXT("Logo.ico"),
    IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    RegisterClass(&wc);
    RECT desired = {0, 0, 800, 600};
    AdjustWindowRect(&desired, WS_OVERLAPPEDWINDOW, FALSE);

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindow(TEXT("Graph Paper"), TEXT("PixelLogic Graph Paper"),
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
    static GroupBoxParabola *gbParabola = nullptr;
    static GroupBoxImage *gbImage = nullptr;
    static ShowShapesInfo *showShapesInfo = nullptr;
    static ImageManager *imageManager = new ImageManager();
    switch (msg)
    {
    case WM_CREATE:
    {
        HDC hdc = GetDC(hwnd);
        ReleaseDC(hwnd, hdc);

        HMENU hMenuBar = CreateAppMenuBar();
        SetMenu(hwnd, hMenuBar);

        // PlaySound(TEXT("C:\\Users\\Shriraj Mardi\\Downloads\\Interstellar.wav"), NULL,
        //       SND_FILENAME | SND_ASYNC | SND_LOOP);

        gbCircle = new GroupBoxCircle();
        gbCircle->DrawGroupBoxCircle(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        gbLine = new GroupBoxLine();
        gbLine->DrawGroupBoxLine(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        gbEllipse = new GroupBoxEllipse();
        gbEllipse->DrawGroupBoxEllipse(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        gbParabola = new GroupBoxParabola();
        gbParabola->DrawGroupBoxParabola(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        gbImage = new GroupBoxImage();
        gbImage->DrawGroupBoxImage(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        showShapesInfo = new ShowShapesInfo();
        showShapesInfo->DrawShowGroupBox(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        ActiveGroupBox = 0; // Default to Line group box

        ShowWindow(hDGBLine, SW_SHOW);
        ShowWindow(hDGBCircle, SW_HIDE);
        ShowWindow(hDGBEllipse, SW_HIDE);
        ShowWindow(hDGBParabola, SW_HIDE);
        ShowWindow(hDGBImages, SW_HIDE);
        DrawDropDownBox(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
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
        for (Parabola &parabola : parabolaList)
        {
            shapeDrawer.DrawParabola(&parabola);
        }

        for (const auto &floodFillPoint : floodFillPointsList)
        {
            FloodFillCustom(hdc, floodFillPoint.first.x, floodFillPoint.first.y, floodFillPoint.second, BackgroundColors);
        }

        imageManager->DrawAllImages(hdc);

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
        else if (ActiveGroupBox == 3)
        {
            FillBox(hDGBParabola, grayCColor);
        }
        else if (ActiveGroupBox == 4)
        {
            FillBox(hDGBImages, grayCColor);
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
            ShowWindow(hDGBParabola, selectedIndex == 3 ? SW_SHOW : SW_HIDE);
            ShowWindow(hDGBImages, selectedIndex == 4 ? SW_SHOW : SW_HIDE);
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
        }

        if (LOWORD(wParam) == 2002 && HIWORD(wParam) == CBN_SELCHANGE)
        {
            int selectedIndex = SendMessage(hDropDownFeature, CB_GETCURSEL, 0, 0);
            ActiveFeature = selectedIndex; // Update active feature based on selection

            if (ActiveFeature == 0)
            {
                HCURSOR hCustomCursor = LoadCursor(NULL, IDC_ARROW);
                SetClassLongPtr(hMain, GCLP_HCURSOR, (LONG_PTR)hCustomCursor);
            }

            if (ActiveFeature == 1)
            {
                HCURSOR hCustomCursor = LoadCursorFromFile(TEXT("target.cur"));
                SetClassLongPtr(hMain, GCLP_HCURSOR, (LONG_PTR)hCustomCursor);
            }

            else if (ActiveFeature == 2)
            {
                HCURSOR hCustomCursor = LoadCursorFromFile(TEXT("Bucket.cur"));
                SetClassLongPtr(hMain, GCLP_HCURSOR, (LONG_PTR)hCustomCursor);
            }
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
        }

        if (LOWORD(wParam) == 1001) // Open menu item
        {
            const TCHAR *selectedFile = ShowOpenFileDialog(hwnd);
            if (selectedFile)
            {
#ifdef UNICODE
                wcstombs(filename, selectedFile, sizeof(filename));
#else
                strncpy(filename, selectedFile, sizeof(filename));
                filename[sizeof(filename) - 1] = '\0';
#endif

                Parsefile(hwnd, filename);
            }
            break;
        }

        if (LOWORD(wParam) == 1002) // Save menu item
        {
            // Show Save As dialog
            ShowSaveImageFileDialog(hwnd);
            break;
        }

        if (LOWORD(wParam) == 1003) // Save Log menu item
        {
            ShowSaveFileDialog(hwnd);
            break;
        }

        if (LOWORD(wParam) == 1101)
        {
            CreateDialogWindowAboutMenu(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        }

        if (LOWORD(wParam) == 1201)
        {
            int response = MessageBox(hwnd, TEXT("Do you really want to reset the Graph?"), TEXT("Confirm"), MB_YESNO | MB_ICONQUESTION);
            if (response == IDYES)
            {
                lineList.clear();
                CircleList.clear();
                ellipseList.clear();
                parabolaList.clear();
                actionLog.clear();
                floodFillPointsList.clear();
                g_images.clear();
                g_arrows.Clear();
            }
            InvalidateRect(hwnd, NULL, TRUE);
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
        if (hCtrl == hDGBCircle || hCtrl == hDGBLine || hCtrl == hDGBEllipse || hCtrl == hDGBParabola || hCtrl == hDGBImages)
        {
            // Set the background color for the group boxes
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
            std::pair<POINT, COLORREF> floodFillPoint(point, ReverseColor);
            AddFloodFillPoint(floodFillPoint);
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
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
        if (ActiveFeature == 0)
        {
            g_drawing = false; // Stop drawing arrows on right-click
            ActiveFeature = -1;
            InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
        }
        else if (ActiveFeature == -1)
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