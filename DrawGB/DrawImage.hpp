#include <windows.h>
#include "../Globals.hpp"
#include "../ImageLoading/Image.hpp"
#include "../ColorPicker.hpp"
WNDPROC OldDGBProcImage = NULL;
WNDPROC OldEditProcImage[4] = {nullptr, nullptr, nullptr, nullptr};

class GroupBoxImage
{
private:
    HWND hEdits[4];

public:
    static TCHAR selectedFile[400];
    static TCHAR *ShowOpenFileDialog(HWND hwnd)
    {
        static TCHAR szFile[MAX_PATH] = TEXT("");
        OPENFILENAME ofn = {0};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = GroupBoxImage::selectedFile;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = TEXT("Bitmap Images (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0");
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn))
            return GroupBoxImage::selectedFile;
        else
            return nullptr;
    }

    void DrawGroupBoxImage(HWND hwnd, HINSTANCE hInstance)
    {
        // Create the group box
        // Create the group box
        hDGBImages = CreateWindowEx(
            0, TEXT("BUTTON"), TEXT("Draw Group Box"),
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
            hwnd, (HMENU)DGBIndexesImages + 0, hInstance, NULL);
        OldDGBProcImage = (WNDPROC)SetWindowLongPtr(hDGBImages, GWLP_WNDPROC, (LONG_PTR)DGBProcImage);
        // Size and positioning
        int boxW = 60, boxH = 20, pad = 10;
        int labelW = 25;
        int xLeft = 10, xRight = 125;
        int y = 30; // start from top margin

        CreateWindowEx(0, TEXT("STATIC"), TEXT("X1:"), WS_CHILD | WS_VISIBLE,
                       xLeft, y, labelW, boxH, hDGBImages, NULL, hInstance, NULL);
        hEdits[0] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xLeft + labelW, y, boxW, boxH, hDGBImages, (HMENU)(DGBIndexesImages + 1), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("Y1:"), WS_CHILD | WS_VISIBLE,
                       xRight, y, labelW, boxH, hDGBImages, NULL, hInstance, NULL);
        hEdits[1] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xRight + labelW, y, boxW, boxH, hDGBImages, (HMENU)(DGBIndexesImages + 2), hInstance, NULL);

        // --- Row 2: X2 / Y2 ---
        y += boxH + pad;
        CreateWindowEx(0, TEXT("STATIC"), TEXT("W:"), WS_CHILD | WS_VISIBLE,
                       xLeft, y, labelW, boxH, hDGBImages, NULL, hInstance, NULL);
        hEdits[2] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xLeft + labelW, y, boxW, boxH, hDGBImages, (HMENU)(DGBIndexesImages + 3), hInstance, NULL);

        CreateWindowEx(0, TEXT("STATIC"), TEXT("H:"), WS_CHILD | WS_VISIBLE,
                       xRight, y, labelW, boxH, hDGBImages, NULL, hInstance, NULL);
        hEdits[3] = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   xRight + labelW, y, boxW, boxH, hDGBImages, (HMENU)(DGBIndexesImages + 4), hInstance, NULL);

        // --- Row 3: Buttons ---
        y += boxH + pad;
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Open Image"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xLeft + 10, y, 100, boxH, hDGBImages, (HMENU)(1), hInstance, NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Submit"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       xRight + 10, y, 100, boxH, hDGBImages, (HMENU)(DGBIndexesImages + 5), hInstance, NULL);

        y += boxH + pad;
        CreateWindowEx(0, TEXT("EDIT"), TEXT("Selected Image:"),
                       WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER, xLeft + 10, y, 300, boxH, hDGBImages, (HMENU)(DGBIndexesImages + 6), hInstance, NULL);

        for (int i = 0; i < 4; ++i)
        {
            OldEditProcImage[i] = (WNDPROC)SetWindowLongPtr(hEdits[i], GWLP_WNDPROC, (LONG_PTR)EditProcImage);
        }
    }

    static LRESULT CALLBACK DGBProcImage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == DGBIndexesImages + 5)
            {
                ImageItem temp = CollectImage(hwnd);
                AddImage(temp);
                InvalidateRect(hShowInfo, NULL, TRUE);
                MessageBox(hwnd, TEXT("Image Submitted!"), TEXT("Info"), MB_OK);
                // SendMessage(hLineDBGB, WM_MSG_DB, 0, 0);
                InvalidateRect(hMain, NULL, TRUE);
            }

            else if (LOWORD(wParam) == 1) // Open Image button
            {
                GroupBoxImage::ShowOpenFileDialog(hwnd);
                if (selectedFile && *selectedFile)
                {
                    LPCTSTR fname = selectedFile;
                    if (LPCTSTR p = _tcsrchr(selectedFile, TEXT('\\')))
                        fname = p + 1; // Extract filename

                    TCHAR finalText[MAX_PATH + 50];
                    _sntprintf(finalText, MAX_PATH + 50, TEXT("Selected Image: %s"), fname);

                    SetWindowText(GetDlgItem(hwnd, DGBIndexesImages + 6), finalText);
                    MessageBox(hwnd, TEXT("Image loaded successfully!"), TEXT("Info"), MB_OK);
                }

                else
                {
                    MessageBox(hwnd, TEXT("Failed to load image."), TEXT("Error"), MB_OK | MB_ICONERROR);
                }
            }

            break;

        case WM_CTLCOLORSTATIC:
        {
            return ColorBG::ColorBGTransparent((HDC)wParam, grayCColor, yellowCColor);
        }
        default:
            return CallWindowProc(OldDGBProcImage, hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    static ImageItem CollectImage(HWND hwndGroupBox)
    {
        char buffer[16];
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;

        HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexesImages + 1);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        x1 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesImages + 2);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        y1 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesImages + 3);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        x2 = atof(buffer);

        hEdit = GetDlgItem(hwndGroupBox, DGBIndexesImages + 4);
        GetWindowTextA(hEdit, buffer, sizeof(buffer));
        y2 = atof(buffer);

        for (int i = 1; i <= 4; i++)
        {
            SetWindowTextA(GetDlgItem(hwndGroupBox, DGBIndexesImages + i), "");
        }

// #ifdef UNICODE
//         const wchar_t *path = GroupBoxImage::selectedFile; // TCHAR == wchar_t
// #else
//         wchar_t wbuf[MAX_PATH];
//         // Ensure GroupBoxImage::selectedFile is null‑terminated!
//         int n = MultiByteToWideChar(CP_ACP, 0,
//                                     GroupBoxImage::selectedFile, -1,
//                                     wbuf, sizeof(wbuf) / sizeof(wbuf[0]));
//         if (n == 0)
//         {
//             MessageBoxA(hwndGroupBox, "Path conversion failed.", "Error", MB_OK | MB_ICONERROR);
//             // return ImageItem(); // return empty / error item (adjust as needed)
//         }
//         const wchar_t *path = wbuf;
// #endif

//         // Optional guard: ensure we have something and it ends in .bmp (case‑insensitive)
//         if (!path || !*path)
//         {
// #ifdef UNICODE
//             MessageBoxW(hwndGroupBox, L"No file selected.", L"Error", MB_OK | MB_ICONERROR);
// #else
//             MessageBoxA(hwndGroupBox, "No file selected.", "Error", MB_OK | MB_ICONERROR);
// #endif
//             // return ImageItem();
//         }

//         // Debug: show the path you're actually sending
// #ifdef UNICODE
//         // Comment out after testing
//         // MessageBoxW(hwndGroupBox, path, L"Using Path", MB_OK);
// #else
//         // MessageBoxA(hwndGroupBox, GroupBoxImage::selectedFile, "Using Path", MB_OK);
// #endif
//         GroupBoxImage::selectedFile[0] = '\0'; // Clear the static buffer after use
        return ImageItem::Create(GroupBoxImage::selectedFile,
                                 static_cast<int>(x1), static_cast<int>(y1),
                                 static_cast<int>(x2), static_cast<int>(y2));
    }

    static LRESULT CALLBACK EditProcImage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ((msg == WM_KEYDOWN || msg == WM_CHAR) && wParam == VK_RETURN)
        {
            if (msg == WM_KEYDOWN)
            {
                // Find which edit box this is
                for (int i = 0; i < 4; ++i)
                {
                    if (hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesImages + 1 + i))
                    {
                        if (i < 3)
                        {
                            HWND hNext = GetDlgItem(GetParent(hwnd), DGBIndexesImages + 2 + i);
                            if (hNext)
                                SetFocus(hNext);
                        }
                        else
                        {
                            // Optionally, trigger submit or do nothing
                            HWND hSubmit = GetDlgItem(GetParent(hwnd), DGBIndexesImages + 5);
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
            OldEditProcImage[hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesImages + 1) ? 0 : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesImages + 2) ? 1
                                                                                         : hwnd == GetDlgItem(GetParent(hwnd), DGBIndexesImages + 3)   ? 2
                                                                                                                                                       : 3],
            hwnd, msg, wParam, lParam);
    }
};

TCHAR GroupBoxImage::selectedFile[400] = TEXT("");