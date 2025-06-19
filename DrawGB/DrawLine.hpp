#include <windows.h>
#include "../Globals.hpp"
#include "../Line.hpp"
#include "../ColorPicker.hpp"
WNDPROC OldDGBProcLine = NULL;

class GroupBoxLine
{
public:
    void DrawGroupBoxLine(HWND hwnd, HINSTANCE hInstance)
    {
        // Create the group box
        // Create the group box
        hDGBLine = CreateWindowEx(
            0, TEXT("BUTTON"), TEXT("Draw Group Box"),
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
            hwnd, (HMENU)DGBIndexesLine + 0, hInstance, NULL);
        OldDGBProcLine = (WNDPROC)SetWindowLongPtr(hDGBLine, GWLP_WNDPROC, (LONG_PTR)DGBProcLine);
        // Size and positioning
        int boxW = 60, boxH = 20, pad = 10;
        int labelW = 25;
        int xLeft = 10, xRight = 125;
        int y = 30; // start from top margin

        CreateWindowEx(0, TEXT("STATIC"), TEXT("X1:"), WS_CHILD | WS_VISIBLE,
                       xLeft, y, labelW, boxH, hDGBLine, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 1), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y1:"), WS_CHILD | WS_VISIBLE,
                       xRight, y, labelW, boxH, hDGBLine, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xRight + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 2), hInstance, NULL);

        // --- Row 2: X2 / Y2 ---
        y += boxH + pad;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("X2:"), WS_CHILD | WS_VISIBLE,
                       xLeft, y, labelW, boxH, hDGBLine, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 3), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y2:"), WS_CHILD | WS_VISIBLE,
                       xRight, y, labelW, boxH, hDGBLine, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xRight + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 4), hInstance, NULL);

        // --- Row 3: Buttons ---
        y += boxH + pad;
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Color Picker"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xLeft + 10, y, 100, boxH, hDGBLine, (HMENU)(1), hInstance, NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Submit"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xRight + 10, y, 100, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 5), hInstance, NULL);
    }

    static LRESULT CALLBACK DGBProcLine(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == DGBIndexesLine + 5)
            {
                Line temp = GroupBoxLine::CollectLine(hwnd);
                temp.setColor(currColor); // Assuming currColor is defined globally or passed in some way
                AddLine(temp);
                // SendMessage(hLineDBGB, WM_MSG_DB, 0, 0);
                InvalidateRect(hMain, NULL, TRUE);
            }
            else if (LOWORD(wParam) == 1)
            {
                CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hDGBLine);
            }
            break;

        case WM_CTLCOLORSTATIC:
        {
            return ColorBG::ColorBGTransparent((HDC)wParam, grayCColor, yellowCColor);
        }

        case WM_KEYDOWN:
            if (wParam == VK_RETURN)
            {
                HWND hFocus = GetFocus();
                for (int i = 2; i <= 4; ++i)
                {
                    HWND hEdit = GetDlgItem(hwnd, DGBIndexesCircle + i);
                    if (hFocus == hEdit)
                    {
                        HWND hNext = GetDlgItem(hwnd, DGBIndexesCircle + i + 1);
                        if (hNext && i < 4)
                            SetFocus(hNext);
                        // Optionally, if on last box, you can trigger submit or do nothing
                        return 0;
                    }
                }
            }
            break;
        default:
            return CallWindowProc(OldDGBProcLine, hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    static Line CollectLine(HWND hwndGroupBox)
    {
        char buffer[16];
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;

        HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexesLine + 1);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        x1 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesLine + 2);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        y1 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesLine + 3);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        x2 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesLine + 4);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        y2 = atof(buffer);

        for (int i = 1; i <= 4; i++)
        {
            SetWindowTextA(GetDlgItem(hwndGroupBox, DGBIndexesLine + i), "");
        }

        return Line::Create(x1, y1, x2, y2, currColor);
    }
};