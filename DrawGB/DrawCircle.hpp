#include <windows.h>
#include "../Globals.hpp"
#include "../Circle.hpp"
#include "../ColorPicker.hpp"
WNDPROC OldDGBProcCircle = NULL;
WNDPROC OldEditProcCircle[3] = {nullptr, nullptr, nullptr};
class GroupBoxCircle
{
    private:
    HWND hEdits[3]; // XCenter, YCenter, Radius
public:
    void DrawGroupBoxCircle(HWND hwnd, HINSTANCE hInstance)
    {
        // Create the group box
        hDGBCircle = CreateWindowEx(
            0, TEXT("BUTTON"), TEXT("Draw Group Box Circle"),
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
            hwnd, (HMENU)DGBIndexesCircle + 1, hInstance, NULL);
        OldDGBProcCircle = (WNDPROC)SetWindowLongPtr(hDGBCircle, GWLP_WNDPROC, (LONG_PTR)GroupBoxCircle::DGBProcCircle);

        // Set the background color for the group box
        // Size and positioning
        int boxW = 80, boxH = 20, pad = 5;
        int x = 30, y = 30;

        // --- Row 1: XCenter ---
        CreateWindowEx(0, TEXT("STATIC"), TEXT("XCenter:"), WS_CHILD | WS_VISIBLE,
                       x - 20, y - 2, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       x + 50, y, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexesCircle + 2), hInstance, NULL);

        // --- Row 2: YCenter ---
        y += boxH + pad + 3;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("YCenter:"), WS_CHILD | WS_VISIBLE,
                       x - 20, y - 2, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       x + 50, y, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexesCircle + 3), hInstance, NULL);

        // --- Row 3: Radius ---
        y += boxH + pad + 3;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("Radius:"), WS_CHILD | WS_VISIBLE,
                       x - 20, y - 2, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
        CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                       x + 50, y, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexesCircle + 4), hInstance, NULL);

        // --- Row 4: Color Picker & Submit on the same line ---
        y += boxH + pad + 5;
        int btnW = 90;
        int btnSpacing = 15;

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Color Picker"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       x, y, btnW, boxH,
                       hDGBCircle, (HMENU)(1), GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Submit"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       x + btnW + btnSpacing, y, btnW, boxH,
                       hDGBCircle, (HMENU)(DGBIndexesCircle + 5), GetModuleHandle(NULL), NULL);

        for (int i = 0; i < 4; ++i)
        {
            hEdits[i] = GetDlgItem(hDGBCircle, DGBIndexesCircle + i + 2);
            OldEditProcCircle[i] = (WNDPROC)SetWindowLongPtr(hEdits[i], GWLP_WNDPROC, (LONG_PTR)EditProcCircle);
        }
    }

    static LRESULT CALLBACK DGBProcCircle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == DGBIndexesCircle + 5)
            {
                Circle temp = GroupBoxCircle::CollectCircle(hDGBCircle);
                temp.SetColor(currColor); // Assuming currColor is defined globally or passed in some way
                AddCircle(temp);
                // CIRCLE: 6, 8, 6, 9.5, (222, 184, 135)
                std::string action = "CIRCLE: " + std::to_string(temp.GetCenterX()) + ", " + std::to_string(temp.GetCenterY()) + ", " +
                                     std::to_string(temp.GetRadius()) + ", (" +
                                     std::to_string(GetRValue(temp.GetColor())) + ", " +
                                     std::to_string(GetGValue(temp.GetColor())) + ", " +
                                     std::to_string(GetBValue(temp.GetColor())) + ")";
                CodeAction(action);
                InvalidateRect(hShowInfo, NULL, TRUE);
                InvalidateRect(hMain, NULL, TRUE);
            }
            else if (LOWORD(wParam) == 1)
            {
                CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hDGBCircle);
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
            return CallWindowProc(OldDGBProcCircle, hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    static Circle CollectCircle(HWND hwndGroupBox)
    {
        char buffer[16];
        float centerX = 0.0f, centerY = 0.0f, radius = 0.0f;

        HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexesCircle + 2);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        centerX = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesCircle + 3);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        centerY = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesCircle + 4);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        radius = atof(buffer);

        for (int i = 2; i <= 4; i++)
        {
            SetWindowTextA(GetDlgItem(hwndGroupBox, DGBIndexesCircle + i), "");
        }

        return Circle::Create(centerX, centerY, radius, currColor);
    }

    static LRESULT CALLBACK EditProcCircle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ((msg == WM_KEYDOWN || msg == WM_CHAR) && wParam == VK_RETURN)
        {
            if (msg == WM_KEYDOWN)
            {
                // Find which edit box this is
                for (int i = 0; i < 3; ++i)
                {
                    if (hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesCircle + 2 + i))
                    {
                        if (i < 2)
                        {
                            // Move focus to the next edit box
                            HWND hNext = GetDlgItem(GetParent(hwnd), DGBIndexesCircle + 3 + i);
                            if (hNext)
                                SetFocus(hNext);
                        }
                        else
                        {
                            // Optionally, trigger submit or do nothing
                            HWND hSubmit = GetDlgItem(GetParent(hwnd), DGBIndexesCircle + 5);
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
            OldEditProcCircle[hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesCircle + 2) ? 0 : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesCircle + 3) ? 1
                                                                                      : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesCircle + 4)   ? 2
                                                                                                                                                  : 3],
            hwnd, msg, wParam, lParam);
    }
};