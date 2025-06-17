#include <windows.h>
#include "../Globals.hpp"
#include "../Circle.hpp"

Circle CollectCircle(HWND hwndGroupBox);
LRESULT CALLBACK DGBProcCircle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC OldDGBProcCircle;

Circle CollectCircle(HWND hwndGroupBox)
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

    return Circle::Create(GetDC(hwndGroupBox), centerX, centerY, radius, RGB(0, 0, 0));
}

void DrawGroupBoxCircle(HWND hwnd, HINSTANCE hInstance)
{

    // Create the group box
    hDGBCircle = CreateWindowEx(
        0, TEXT("BUTTON"), TEXT("Draw Group Box Circle"),
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
        hwnd, (HMENU)DGBIndexesCircle + 1, hInstance, NULL);
    OldDGBProcCircle = (WNDPROC)SetWindowLongPtr(hDGBCircle, GWLP_WNDPROC, (LONG_PTR)DGBProcCircle);

    // Set the background color for the group box
    HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
    SetClassLongPtr(hDGBCircle, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
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
}

LRESULT CALLBACK DGBProcCircle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == DGBIndexesCircle + 5)
        {
            Circle temp = CollectCircle(hDGBCircle);
            temp.SetColor(currColor); // Assuming currColor is defined globally or passed in some way
            AddCircle(temp);
            MessageBox(hwnd, TEXT("Circle added!"), TEXT("Info"), MB_OK | MB_ICONINFORMATION);
            // SendMessage(hCircleDBGB, WM_MSG_DB, 0, 0);
            InvalidateRect(hMain, NULL, TRUE);
        }
        // else if (LOWORD(wParam) == 1)
        // {
        //     CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hDGBCircle);
        // }
        break;

    default:
        return CallWindowProc(OldDGBProcCircle, hwnd, msg, wParam, lParam);
    }
    return 0;
}