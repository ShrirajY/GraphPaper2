#include <windows.h>
#include "../Globals.hpp"

void DrawDropDownBox(HWND hwnd, HINSTANCE hInstance);

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
}