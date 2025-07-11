#include <windows.h>
#include "../Globals.hpp"
#include "../Line.hpp"
#include "../ColorPicker.hpp"
WNDPROC OldDGBProcLine = NULL;
WNDPROC OldEditProcLine[4] = {nullptr, nullptr, nullptr, nullptr};
class GroupBoxLine
{
private:
    
    HWND hEdits[4];

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
        hEdits[0] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xLeft + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 1), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y1:"), WS_CHILD | WS_VISIBLE,
                       xRight, y, labelW, boxH, hDGBLine, NULL, hInstance, NULL);
        hEdits[1] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xRight + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 2), hInstance, NULL);

        // --- Row 2: X2 / Y2 ---
        y += boxH + pad;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("X2:"), WS_CHILD | WS_VISIBLE,
                       xLeft, y, labelW, boxH, hDGBLine, NULL, hInstance, NULL);
        hEdits[2] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xLeft + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 3), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y2:"), WS_CHILD | WS_VISIBLE,
                       xRight, y, labelW, boxH, hDGBLine, NULL, hInstance, NULL);
        hEdits[3] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xRight + labelW, y, boxW, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 4), hInstance, NULL);

        // --- Row 3: Buttons ---
        y += boxH + pad;
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Color Picker"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xLeft + 10, y, 100, boxH, hDGBLine, (HMENU)(1), hInstance, NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Submit"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xRight + 10, y, 100, boxH, hDGBLine, (HMENU)(DGBIndexesLine + 5), hInstance, NULL);
        for (int i = 0; i < 4; ++i)
        {
            OldEditProcLine[i] = (WNDPROC)SetWindowLongPtr(hEdits[i], GWLP_WNDPROC, (LONG_PTR)EditProcLine);
        }
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
                // LINE: 6, 8, 6, 9.5, (222, 184, 135)
                std::string action = "LINE: " + std::to_string(temp.getX1()) + ", " + std::to_string(temp.getY1()) + ", " +
                                     std::to_string(temp.getX2()) + ", " + std::to_string(temp.getY2()) + ", (" +
                                     std::to_string(GetRValue(temp.getColor())) + ", " + std::to_string(GetGValue(temp.getColor())) + ", " +
                                     std::to_string(GetBValue(temp.getColor())) + ")";
                CodeAction(action);
                InvalidateRect(hShowInfo, NULL, TRUE);
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

    static LRESULT CALLBACK EditProcLine(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ((msg == WM_KEYDOWN || msg == WM_CHAR) && wParam == VK_RETURN)
        {
            if (msg == WM_KEYDOWN)
            {
                // Find which edit box this is
                for (int i = 0; i < 4; ++i)
                {
                    if (hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesLine + 1 + i))
                    {
                        if (i < 3)
                        {
                            HWND hNext = GetDlgItem(GetParent(hwnd), DGBIndexesLine + 2 + i);
                            if (hNext)
                                SetFocus(hNext);
                        }
                        else
                        {
                            // Optionally, trigger submit or do nothing
                            HWND hSubmit = GetDlgItem(GetParent(hwnd), DGBIndexesLine + 5);
                            SendMessage(hSubmit, BM_CLICK, 0, 0);
                        }
                        break;
                    }
                }
            }
            // Always return non-zero for both WM_KEYDOWN and WM_CHAR to suppress beep
            return 1;
        }
        return CallWindowProc(
            OldEditProcLine[hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesLine + 1) ? 0 : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesLine + 2) ? 1
                                                                                      : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesLine + 3)   ? 2
                                                                                                                                                  : 3],
            hwnd, msg, wParam, lParam);
    }
};
