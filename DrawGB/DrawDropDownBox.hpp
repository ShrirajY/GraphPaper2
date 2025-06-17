#include <windows.h>
#include "../Globals.hpp"

void DrawDropDownBox(HWND hwnd, HINSTANCE hInstance);

LRESULT CALLBACK DDBProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC OldDropDownProc;
void DrawDropDownBox(HWND hwnd, HINSTANCE hInstance)
{
    // Create the drop-down box
    hDropDown = CreateWindow("COMBOBOX", NULL,
                                      WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                                      DGroupBoxLeft, DGroupBoxTop - 30, 260, 100, hwnd, (HMENU)2001,
                                      (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

    // Set the background color for the drop-down box
    HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
    SetClassLongPtr(hDropDown, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

    // Add items to the drop-down box
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Line"));
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Circle"));
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Ellipse"));

    // Set the default selection
    SendMessage(hDropDown, CB_SETCURSEL, 0, 0);

    // Set the window procedure for the drop-down box
    OldDropDownProc = (WNDPROC)SetWindowLongPtr(hDropDown, GWLP_WNDPROC, (LONG_PTR)DDBProc);
}

LRESULT CALLBACK DDBProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            // Handle selection change
            int index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
            TCHAR buffer[256];
            SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)buffer);
            MessageBox(hwnd, buffer, TEXT("Selected Shape"), MB_OK);
        }
        break;

    default:
        return CallWindowProc(OldDropDownProc, hwnd, msg, wParam, lParam);
    }
    return 0;
}