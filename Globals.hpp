#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <windows.h>

HWND hMain = NULL; // Handle for the main window

const int tickSpacing = 25; // Spacing between grid lines in pixels
const int tickSize = 5;     // Size of the ticks in pixels

const int GraphWidth = 800;  // Width of the graph area in pixels
const int GraphHeight = 600; // Height of the graph area in pixels

const int DGBIndexesLine = 1000;   // Base index for dialog group box controls
const int DGBIndexesCircle = 1100; // Base index for circle controls
const int DGBIndexesEllipse = 1200; // Base index for ellipse controls
// Group Box for Drawing
int DGBwidth = 250;
int DGBHeight = 150;
int ShowInfoGBWidth = 350;
int ShowInfoGBHeight = 400;
int DDBwidth = 260; // Width of the drop-down box
int DDBHeight = 100; // Height of the drop-down box

int DGroupBoxLeft = 800 + 30;
int DGroupBoxTop = 40;

int ShowInfoLeft = 800 + 30;
int ShowInfoTop = 220;

// Handles for Group Boxes
HWND hDGBCircle = NULL;  // Handle for the Circle group box
HWND hDGBLine = NULL;    // Handle for the Line group box
HWND hDGBEllipse = NULL; // Handle for the Ellipse group box
HWND hDropDown = NULL;   // Handle for the drop-down box
HWND hDropDownFeature = NULL; // Handle for the feature drop-down box
HWND hColorPicker = NULL;
HWND hPrevEdit = NULL; // Handle for the previous edit control
HWND hShowInfo = NULL; // Handle for the Show Info group box


COLORREF currColor = RGB(0, 0, 0); // Current color for drawing shapes
COLORREF redCColor = RGB(230, 0, 0);
COLORREF greenCColor = RGB(0, 230, 0);
COLORREF blueCColor = RGB(0, 0, 230);
COLORREF yellowCColor = RGB(230, 230, 0);
COLORREF blackCColor = RGB(0, 0, 0);
COLORREF whiteCColor = RGB(255, 255, 255);
COLORREF grayCColor = RGB(100, 100, 100);
COLORREF violetCColor = RGB(128, 0, 128);
COLORREF candyCColor = RGB(255, 128, 128);
HBRUSH hTransparentBrush = NULL; // Brush for transparent background



void FillBox(HWND hwnd, COLORREF color)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    HBRUSH hBrush = CreateSolidBrush(color); // Your color
    FillRect(GetDC(hwnd), &rc, hBrush);
}

INT_PTR ColorBG(HDC hdc, COLORREF bgColor, COLORREF textColor)
{
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, bgColor);
    SetTextColor(hdc, textColor);
    static HBRUSH hBrush = CreateSolidBrush(bgColor);
    return (INT_PTR)hBrush;
}

INT_PTR ColorBGTransparent(HDC hdc, COLORREF bgColor, COLORREF textColor)
{
    SetBkMode(hdc, TRANSPARENT);
    SetBkColor(hdc, bgColor);
    SetTextColor(hdc, textColor);
    static HBRUSH hBrush = CreateSolidBrush(bgColor);
    return (INT_PTR)hBrush;
}

class ColorBG
{
    public:
    static INT_PTR ColorBGOpaque(HDC hdc, COLORREF bgColor, COLORREF textColor)
    {
        SetBkMode(hdc, OPAQUE);
        SetBkColor(hdc, bgColor);
        SetTextColor(hdc, textColor);
        static HBRUSH hBrush = CreateSolidBrush(bgColor);
        return (INT_PTR)hBrush;
    }

    static INT_PTR ColorBGTransparent(HDC hdc, COLORREF bgColor, COLORREF textColor)
    {
        SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, bgColor);
        SetTextColor(hdc, textColor);
        static HBRUSH hBrush = CreateSolidBrush(bgColor);
        return (INT_PTR)hBrush;
    }
};

int ActiveGroupBox = 0; // 0 for Line, 1 for Circle, 2 for Ellipse
int ActiveFeature = 0; // 0 for Draw Arrows, 1 for Fill Color, 2 for Select Shape

int SelectedShape = 0; // 1 for Line, 2 for Circle, 3 for Ellipse
// In your window class or as globals:
COLORREF OldColor = RGB(0, 0, 0); // Default color for shapes
#endif // GLOBALS_HPP