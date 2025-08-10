#include <windows.h>
#include "../Globals.hpp"
#include "../Parabola.hpp"
#include "../ColorPicker.hpp"
WNDPROC OldDGBProcParabola = NULL;
WNDPROC OldEditProcParabola[6] = {nullptr, nullptr, nullptr, nullptr, nullptr};
class GroupBoxParabola
{
private:
    HWND hEdits[6]; // VertexX, VertexY, A, B, Angle
public:
    void DrawGroupBoxParabola(HWND hwnd, HINSTANCE hInstance)
    {
        hDGBParabola = CreateWindowEx(
            0, TEXT("BUTTON"), TEXT("Draw Group Box Parabola"),
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
            hwnd, (HMENU)(DGBIndexesParabola + 0), hInstance, NULL);

        OldDGBProcParabola = (WNDPROC)SetWindowLongPtr(hDGBParabola, GWLP_WNDPROC, (LONG_PTR)DGBProcParabola);

        int boxW = 60, boxH = 20, labelW = 50, pad = 6;
        int xLeft = 10, xRight = 130, yLine = 20, lineGap = boxH + pad;

        // --- Row 1: Endpoint 1 (X1, Y1) ---
        CreateWindowEx(0, TEXT("STATIC"), TEXT("X1:"), WS_CHILD | WS_VISIBLE,
                       xLeft, yLine, labelW, boxH, hDGBParabola, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, yLine, boxW, boxH, hDGBParabola, (HMENU)(DGBIndexesParabola + 1), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y1:"), WS_CHILD | WS_VISIBLE,
                       xRight, yLine, labelW, boxH, hDGBParabola, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xRight + labelW, yLine, boxW, boxH, hDGBParabola, (HMENU)(DGBIndexesParabola + 2), hInstance, NULL);

        // --- Row 2: Vertex (VX, VY) ---
        yLine += lineGap;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("VX:"), WS_CHILD | WS_VISIBLE,
                       xLeft, yLine, labelW, boxH, hDGBParabola, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, yLine, boxW, boxH, hDGBParabola, (HMENU)(DGBIndexesParabola + 3), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("VY:"), WS_CHILD | WS_VISIBLE,
                       xRight, yLine, labelW, boxH, hDGBParabola, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xRight + labelW, yLine, boxW, boxH, hDGBParabola, (HMENU)(DGBIndexesParabola + 4), hInstance, NULL);

        // --- Row 3: Endpoint 2 (X2, Y2) ---
        yLine += lineGap;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("X2:"), WS_CHILD | WS_VISIBLE,
                       xLeft, yLine, labelW, boxH, hDGBParabola, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xLeft + labelW, yLine, boxW, boxH, hDGBParabola, (HMENU)(DGBIndexesParabola + 5), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y2:"), WS_CHILD | WS_VISIBLE,
                       xRight, yLine, labelW, boxH, hDGBParabola, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       xRight + labelW, yLine, boxW, boxH, hDGBParabola, (HMENU)(DGBIndexesParabola + 6), hInstance, NULL);

        // --- Row 4: Color Picker & Submit Button ---
        yLine += lineGap;
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Color Picker"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xLeft, yLine, 100, boxH, hDGBParabola, (HMENU)(1), hInstance, NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Submit"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xRight, yLine, 100, boxH, hDGBParabola, (HMENU)(DGBIndexesParabola + 7), hInstance, NULL);

        // Optional: Subclass edit controls if needed
        for (int i = 0; i < 6; ++i)
        {
            hEdits[i] = GetDlgItem(hDGBParabola, DGBIndexesParabola + i + 1);
            OldEditProcParabola[i] = (WNDPROC)SetWindowLongPtr(hEdits[i], GWLP_WNDPROC, (LONG_PTR)EditProcParabola);
        }
    }

    static LRESULT CALLBACK DGBProcParabola(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == DGBIndexesParabola + 7) // Submit
            {
                Parabola temp = GroupBoxParabola::CollectParabola(hwnd);
                temp.setColor(currColor);
                AddParabola(temp);

                std::string action = "PARABOLA: " + std::to_string(temp.getX1()) + ", " + std::to_string(temp.getY1()) + ", " + std::to_string(temp.getVertexX()) + ", " + std::to_string(temp.getVertexY()) + ", " + std::to_string(temp.getX2()) + ", " + std::to_string(temp.getY2()) + ", (" + std::to_string(GetRValue(temp.getColor())) + ", " + std::to_string(GetGValue(temp.getColor())) + ", " + std::to_string(GetBValue(temp.getColor())) + ")";
                CodeAction(action);

                InvalidateRect(hShowInfo, NULL, TRUE);
                InvalidateRect(hMain, NULL, TRUE);
            }
            else if (LOWORD(wParam) == 1) // Color Picker
            {
                CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hwnd);
            }
            break;

        case WM_CTLCOLORSTATIC:
            return ColorBG::ColorBGTransparent((HDC)wParam, grayCColor, yellowCColor);

        case WM_KEYDOWN:
            if (wParam == VK_RETURN)
            {
                HWND hFocus = GetFocus();
                for (int i = 1; i <= 6; ++i)
                {
                    HWND hEdit = GetDlgItem(hwnd, DGBIndexesParabola + i);
                    if (hFocus == hEdit)
                    {
                        HWND hNext = GetDlgItem(hwnd, DGBIndexesParabola + i + 1);
                        if (hNext && i < 6)
                            SetFocus(hNext);
                        return 0;
                    }
                }
            }
            break;

        default:
            return CallWindowProc(OldDGBProcParabola, hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    static Parabola CollectParabola(HWND hwndGroupBox)
    {
        char buffer[16];
        float x1 = 0.0f, y1 = 0.0f, vx = 0.0f, vy = 0.0f, x2 = 0.0f, y2 = 0.0f;

        HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexesParabola + 1); // X1
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        x1 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesParabola + 2); // Y1
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        y1 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesParabola + 3); // VX
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        vx = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesParabola + 4); // VY
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        vy = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesParabola + 5); // X2
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        x2 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesParabola + 6); // Y2
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        y2 = atof(buffer);

        // Clear all input fields
        for (int i = 1; i <= 6; i++)
            SetWindowTextA(GetDlgItem(hwndGroupBox, DGBIndexesParabola + i), "");

        // Call your parabola constructor using 3 points
        return Parabola::CreateFrom3Points(x1, y1, vx, vy, x2, y2, currColor);
    }

    static LRESULT CALLBACK EditProcParabola(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ((msg == WM_KEYDOWN || msg == WM_CHAR) && wParam == VK_RETURN)
        {
            if (msg == WM_KEYDOWN)
            {
                for (int i = 0; i < 6; ++i)
                {
                    if (hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesParabola + 1 + i))
                    {
                        if (i < 5)
                        {
                            HWND hNext = GetDlgItem(GetParent(hwnd), DGBIndexesParabola + 2 + i);
                            if (hNext)
                                SetFocus(hNext);
                        }
                        else
                        {
                            HWND hSubmit = GetDlgItem(GetParent(hwnd), DGBIndexesParabola + 7);
                            SendMessage(hSubmit, BM_CLICK, 0, 0);
                        }
                        break;
                    }
                }
            }
            return 1;
        }

        int index = -1;
        for (int i = 0; i < 6; ++i)
        {
            if (hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesParabola + 1 + i))
            {
                index = i;
                break;
            }
        }

        if (index != -1)
            return CallWindowProc(OldEditProcParabola[index], hwnd, msg, wParam, lParam);

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
};