#include <cmath>
#include <windows.h>
#include "../Circle.hpp"
#include "../Line.hpp"
#include "../Ellipse.hpp"
#include "../Globals.hpp"
WNDPROC OldShowInfoProc = NULL;

enum class ShapeInfoType
{
    Line,
    Circle
};

class ShowShapesInfo
{
public:
    ShowShapesInfo() = default;
    ~ShowShapesInfo() = default;

    void DrawShowGroupBox(HWND hwnd, HINSTANCE hInstance)
    {
        hShowInfo = CreateWindowEx(
            0, TEXT("BUTTON"), TEXT("Show Shapes Information"),
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            ShowInfoLeft, ShowInfoTop, ShowInfoGBWidth, ShowInfoGBHeight,
            hwnd, (HMENU)2500, hInstance, NULL);
        OldShowInfoProc = (WNDPROC)SetWindowLongPtr(hShowInfo, GWLP_WNDPROC, (LONG_PTR)ShowShapesInfo::DGBProcInfo);
    }

    static LRESULT CALLBACK DGBProcInfo(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CTLCOLORSTATIC:
            return ColorBG::ColorBGTransparent((HDC)wParam, grayCColor, yellowCColor);
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            // Example usage: draw line info
            POINT start = {20, 40};
            POINT end = {250, 120};
            // ShowShapesInfo::DrawShapeInfoPanel(hdc, rc, ShapeInfoType::Line, &start, &end);

            int x = 30;
            int y = rc.top + 30;
            int lineHeight = 32;
            DrawStyledBackground(hdc, rc);

            HFONT hFont = CreateInfoFont(25, FW_BOLD);
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(30, 30, 80));
            DrawShapeInfoPanel(hdc, rc);

            // Example usage: draw circle info (uncomment to test)
            // ShowShapesInfo::DrawShapeInfoPanel(hdc, rc, ShapeInfoType::Circle, nullptr, nullptr, 100.0f, 120.0f, 50.0f);

            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            return CallWindowProc(OldShowInfoProc, hwnd, msg, wParam, lParam);
        }
        return 0;
    }

private:
    static void DrawStyledBackground(HDC hdc, RECT &rc)
    {
        HBRUSH hBrush = CreateSolidBrush(RGB(230, 230, 255));
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(120, 120, 200));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        RoundRect(hdc, 5, 5, rc.right - 5, rc.bottom - 5, 20, 20);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }

    static HFONT CreateInfoFont(int height, int weight)
    {
        LOGFONT lf = {0};
        lf.lfHeight = height;
        lf.lfWeight = weight;
        lstrcpy(lf.lfFaceName, TEXT("Segoe UI"));
        return CreateFontIndirect(&lf);
    }

    static void DrawShapeInfoTitle(HDC hdc, int &y, int lineHeight)
    {
        HFONT hFontTitle = CreateInfoFont(28, FW_HEAVY);
        HFONT hOldFontTitle = (HFONT)SelectObject(hdc, hFontTitle);
        COLORREF color;
        LPCTSTR title;
        if (SelectedShape == 1)
        {
            color = RGB(60, 0, 120);
            title = TEXT("Line Information");
        }
        if (SelectedShape == 2)
        {
            color = RGB(0, 120, 60);
            title = TEXT("Circle Information");
        }
        else if (SelectedShape == 3)
        {
            color = RGB(60, 0, 120);
            title = TEXT("Ellipse Information");
        }
        else if(SelectedShape == 0)
        {
            color = RGB(230, 230, 0);
            title = TEXT("Overall Information");
        }
        SetTextColor(hdc, color);
        TextOut(hdc, 30, y, title, lstrlen(title));
        y += lineHeight + 8;

        SelectObject(hdc, hOldFontTitle);
        DeleteObject(hFontTitle);
        SetTextColor(hdc, RGB(30, 30, 80));
    }

    static void DrawColoredBullet(HDC hdc, int x, int y, COLORREF color, int right = 14, int bottom = 20)
    {
        HBRUSH hBrush = CreateSolidBrush(color);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Rectangle(hdc, x, y + 8, x + right, y + bottom); // Small rectangle as bullet
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
    }

    static void DrawLineInfoBullets(HDC hdc, int x, int &y, int lineHeight)
    {
        COLORREF bulletColor = RGB(60, 0, 120);
        char buf[128];

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Start Point: (%.2f, %.2f)", SelectedLine->x1_, SelectedLine->y1_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " End Point:   (%.2f, %.2f)", SelectedLine->x2_, SelectedLine->y2_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Slope:       %.2f", SelectedLine->slope_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Length:      %.2f", SelectedLine->length_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        double c = SelectedLine->y1_ - SelectedLine->slope_ * SelectedLine->x1_;
        snprintf(buf, sizeof(buf), " Equation:    y = %.2fx + %.2f", SelectedLine->slope_, c);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Color: RGB(%d, %d, %d)",
                 GetRValue(OldColor),
                 GetGValue(OldColor),
                 GetBValue(OldColor));
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        DrawColoredBullet(hdc, x + 30, y + 16, OldColor, 30, 30);
        y += lineHeight;
    }

    static void DrawCircleInfoBullets(HDC hdc, int x, int &y, int lineHeight)
    {
        COLORREF bulletColor = RGB(0, 120, 60);
        char buf[128];

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Center: (%.2f, %.2f)", SelectedCircle->centerX_, SelectedCircle->centerY_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Radius: %.2f", SelectedCircle->radius_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Equation:");
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), "    (x - %.2f)^2       (y - %.2f)^2", SelectedCircle->centerX_, SelectedCircle->centerY_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        snprintf(buf, sizeof(buf), "   ------------   +   ------------   = 1");
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), "      %.2f^2             %.2f^2", SelectedCircle->radius_, SelectedCircle->radius_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Color: RGB(%d, %d, %d)",
                 GetRValue(OldColor),
                 GetGValue(OldColor),
                 GetBValue(OldColor));
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        DrawColoredBullet(hdc, x + 30, y + 16, OldColor, 30, 30);
        y += lineHeight;
    }

    static void DrawEllipseInfoBullets(HDC hdc, int x, int &y, int lineHeight)
    {
        COLORREF bulletColor = RGB(120, 60, 0);
        char buf[128];

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Center: (%.2f, %.2f)", SelectedEllipse->centerX_, SelectedEllipse->centerY_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Semi-Major Axis: %.2f", SelectedEllipse->a_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Semi-Minor Axis: %.2f", SelectedEllipse->b_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Rotation Angle: %.2f", SelectedEllipse->angle_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Equation:");
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), "    (x - %.2f)^2       (y - %.2f)^2", SelectedEllipse->centerX_, SelectedEllipse->centerY_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        snprintf(buf, sizeof(buf), "   ------------   +   ------------   = 1");
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), "      %.2f^2             %.2f^2", SelectedEllipse->a_, SelectedEllipse->b_);
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        y += lineHeight;

        DrawColoredBullet(hdc, x, y, bulletColor);
        snprintf(buf, sizeof(buf), " Color: RGB(%d, %d, %d)",
                 GetRValue(OldColor),
                 GetGValue(OldColor),
                 GetBValue(OldColor));
        TextOutA(hdc, x + 20, y, buf, strlen(buf));
        DrawColoredBullet(hdc, x + 30, y + 16, OldColor, 30, 30);
        y += lineHeight;
    }

    static void DrawOverallInfo(HDC hdc, int x, int &y, int lineHeight)
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "- Total Shapes: %d", (int)(lineList.size() + CircleList.size() + ellipseList.size()));
        TextOutA(hdc, x, y, buf, strlen(buf));
        y += lineHeight;
        snprintf(buf, sizeof(buf), "- Total Lines: %d", (int)lineList.size());
        TextOutA(hdc, x, y, buf, strlen(buf));
        y += lineHeight;
        snprintf(buf, sizeof(buf), "- Total Circles: %d", (int)CircleList.size());
        TextOutA(hdc, x, y, buf, strlen(buf));
        y += lineHeight;
        snprintf(buf, sizeof(buf), "- Total Ellipses: %d", (int)ellipseList.size());
        TextOutA(hdc, x, y, buf, strlen(buf));
        y += lineHeight;
    }

    static void DrawShapeInfoPanel(
        HDC hdc, RECT &rc)
    {
        DrawStyledBackground(hdc, rc);

        HFONT hFont = CreateInfoFont(25, FW_BOLD);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(30, 30, 80));

        
        int y = 30, lineHeight = 32;
        DrawShapeInfoTitle(hdc, y, 32);

        if (SelectedShape == 0)
        {
            DrawOverallInfo(hdc, 30, y, lineHeight);
        }
        else if (SelectedShape == 1)
        {
            DrawLineInfoBullets(hdc, 30, y, lineHeight);
        }
        else if (SelectedShape == 2)
        {
            DrawCircleInfoBullets(hdc, 30, y, lineHeight);
        }
        else if (SelectedShape == 3)
        {
            DrawEllipseInfoBullets(hdc, 30, y, lineHeight);
        }

        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }
};