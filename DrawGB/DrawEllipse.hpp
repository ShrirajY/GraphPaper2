#include <windows.h>
#include "../Globals.hpp"
#include "../Ellipse.hpp"
#include "../ColorPicker.hpp"
WNDPROC OldDGBProcEllipse = NULL;
WNDPROC OldEditProcEllipse[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
class GroupBoxEllipse
{
private:
    HWND hEdits[5]; // XCenter, YCenter, A, B, Angle
public:
    void DrawGroupBoxEllipse(HWND hwnd, HINSTANCE hInstance)
    {

        // Create the group box
        hDGBEllipse = CreateWindowEx(
            0, TEXT("BUTTON"), TEXT("Draw Group Box Ellipse"),
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
            hwnd, (HMENU)DGBIndexesEllipse + 0, hInstance, NULL);
        OldDGBProcEllipse = (WNDPROC)SetWindowLongPtr(hDGBEllipse, GWLP_WNDPROC, (LONG_PTR)DGBProcEllipse);
        // Size and positioning
        // int boxW = 80, boxH = 20, pad = 5;
        // int x = 30, y = 30;
        int boxW = 60; // edit box width
        int boxH = 20; // edit box height
        int labelW = 50;
        int pad = 6;
        int x = 10, y = 20; // top-left margin inside the groupbox
        int xLeft = 10;
        int xRight = 130; // Left + half width spacing
        int yLine = y;
        int lineGap = boxH + pad;

        // --- Row 1: XCenter and YCenter ---
        CreateWindowEx(0, TEXT("STATIC"), TEXT("X:"), WS_CHILD | WS_VISIBLE,
                       xLeft, yLine, labelW, boxH, hDGBEllipse, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, yLine, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexesEllipse + 1), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y:"), WS_CHILD | WS_VISIBLE,
                       xRight, yLine, labelW, boxH, hDGBEllipse, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xRight + labelW, yLine, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexesEllipse + 2), hInstance, NULL);

        // --- Row 2: A and B ---
        yLine += lineGap;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("A:"), WS_CHILD | WS_VISIBLE,
                       xLeft, yLine, labelW, boxH, hDGBEllipse, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, yLine, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexesEllipse + 3), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("B:"), WS_CHILD | WS_VISIBLE,
                       xRight, yLine, labelW, boxH, hDGBEllipse, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xRight + labelW, yLine, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexesEllipse + 4), hInstance, NULL);

        // --- Row 3: Angle and Color Picker ---
        yLine += lineGap;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("Ang:"), WS_CHILD | WS_VISIBLE,
                       xLeft, yLine, labelW, boxH, hDGBEllipse, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, yLine, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexesEllipse + 5), hInstance, NULL);

        yLine += lineGap;
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Color Picker"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xLeft, yLine, 100, boxH, hDGBEllipse, (HMENU)(1), hInstance, NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Submit"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xRight, yLine, 100, boxH, hDGBEllipse, (HMENU)(DGBIndexesEllipse + 6), hInstance, NULL);

        for (int i = 0; i < 5; ++i)
        {
            hEdits[i] = GetDlgItem(hDGBEllipse, DGBIndexesEllipse + i + 1);
            OldEditProcEllipse[i] = (WNDPROC)SetWindowLongPtr(hEdits[i], GWLP_WNDPROC, (LONG_PTR)EditProcEllipse);
        }
    }

    static LRESULT CALLBACK DGBProcEllipse(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == DGBIndexesEllipse + 6)
            {
                // MessageBox(hwnd, TEXT("Ellipse Submitted!"), TEXT("Info"), MB_OK);
                Ellipse_ temp = GroupBoxEllipse::CollectEllipse(hwnd);
                temp.setColor(currColor); // Assuming currColor is defined globally or passed in some way
                AddEllipse(temp);

                // ELLIPSE: 4.5, 0.5, 2.5, 0.5, 0, (200, 200, 0)
                std::string action = "ELLIPSE: " + std::to_string(temp.getCenterX()) + ", " + std::to_string(temp.getCenterY()) + ", " +
                                     std::to_string(temp.getA()) + ", " + std::to_string(temp.getB()) + ", " +
                                     std::to_string(temp.getAngle()) + ", (" +
                                     std::to_string(GetRValue(temp.getColor())) + ", " +
                                     std::to_string(GetGValue(temp.getColor())) + ", " +
                                     std::to_string(GetBValue(temp.getColor())) + ")";
                CodeAction(action);
                InvalidateRect(hShowInfo, NULL, TRUE);
                // SendMessage(hEllipseDBGB, WM_MSG_DB, 0, 0);
                InvalidateRect(hMain, NULL, TRUE);
            }
            else if (LOWORD(wParam) == 1)
            {
                CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hDGBEllipse);
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
            return CallWindowProc(OldDGBProcEllipse, hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    static Ellipse_ CollectEllipse(HWND hwndGroupBox)
    {
        char buffer[16];
        float centerX = 0.0f, centerY = 0.0f, a = 0.0f, b = 0.0f, angle = 0.0f;

        HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexesEllipse + 1);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        centerX = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesEllipse + 2);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        centerY = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesEllipse + 3);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        a = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesEllipse + 4);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        b = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesEllipse + 5);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        angle = atof(buffer);

        for (int i = 1; i <= 5; i++)
        {
            SetWindowTextA(GetDlgItem(hwndGroupBox, DGBIndexesEllipse + i), "");
        }

        return Ellipse_::Create(centerX, centerY, a, b, angle, currColor);
    }

    static LRESULT CALLBACK EditProcEllipse(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ((msg == WM_KEYDOWN || msg == WM_CHAR) && wParam == VK_RETURN)
        {
            if (msg == WM_KEYDOWN)
            {
                // Find which edit box this is
                for (int i = 0; i < 5; ++i)
                {
                    if (hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesEllipse + 1 + i))
                    {
                        if (i < 4)
                        {
                            // Move focus to the next edit box
                            HWND hNext = GetDlgItem(GetParent(hwnd), DGBIndexesEllipse + 2 + i);
                            if (hNext)
                                SetFocus(hNext);
                        }
                        else
                        {
                            // Optionally, trigger submit or do nothing
                            HWND hSubmit = GetDlgItem(GetParent(hwnd), DGBIndexesEllipse + 6);
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
            OldEditProcEllipse[hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesEllipse + 1) ? 0
                : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesEllipse + 2) ? 1
                : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesEllipse + 3) ? 2
                : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesEllipse + 4) ? 3
                : 4],
            hwnd, msg, wParam, lParam);
    }
};