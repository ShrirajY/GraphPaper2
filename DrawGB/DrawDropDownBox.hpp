#include <windows.h>
#include "../Globals.hpp"

void DrawDropDownBox(HWND hwnd, HINSTANCE hInstance);

void DrawDropDownBox(HWND hwnd, HINSTANCE hInstance)
{
    // Create the drop-down box
    hDropDown = CreateWindow(TEXT("COMBOBOX"), NULL,
                             WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                             DGroupBoxLeft, DGroupBoxTop - 30, DDBwidth, DDBHeight, hwnd, (HMENU)2001,
                             (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

    // Set the background color for the drop-down box
    HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
    SetClassLongPtr(hDropDown, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

    // Add items to the drop-down box
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Line"));
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Circle"));
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Ellipse"));
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Parabola"));
    SendMessage(hDropDown, CB_ADDSTRING, 0, (LPARAM)TEXT("Image"));

    // Set the default selection
    SendMessage(hDropDown, CB_SETCURSEL, 0, 0);


    hDropDownFeature = CreateWindow(TEXT("COMBOBOX"), NULL,
                             WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                             DGroupBoxLeft + DDBwidth + 20, DGroupBoxTop - 30, DDBwidth, DDBHeight, hwnd, (HMENU)2002,
                             (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

    // Set the background color for the drop-down box
    HBRUSH hBrushFeature = CreateSolidBrush(RGB(128, 128, 128));
    SetClassLongPtr(hDropDownFeature, GCLP_HBRBACKGROUND, (LONG_PTR)hBrushFeature);

    // Add items to the drop-down box
    SendMessage(hDropDownFeature, CB_ADDSTRING, 0, (LPARAM)TEXT("Draw Arrows"));
    SendMessage(hDropDownFeature, CB_ADDSTRING, 0, (LPARAM)TEXT("Select Shape"));
    SendMessage(hDropDownFeature, CB_ADDSTRING, 0, (LPARAM)TEXT("Fill Color"));

    // Set the default selection
    SendMessage(hDropDownFeature, CB_SETCURSEL, 0, 0);
}