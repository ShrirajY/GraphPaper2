#ifndef WINDEV_GRAPH_PAPER2_MENU_HPP
#define WINDEV_GRAPH_PAPER2_MENU_HPP

#include <windows.h>
#include <fstream>
#include "Globals.hpp"

void SaveWindowAsBMP(HWND hwnd, const char* filename);
void SaveLogToFile(const char* filename);
char filename[1000];
const TCHAR* ShowOpenFileDialog(HWND hwnd)
{
    static TCHAR szFile[MAX_PATH] = TEXT("");
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = TEXT("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
        return szFile;
    else
        return nullptr;
}

void ShowSaveImageFileDialog(HWND hwnd)
{
    OPENFILENAME ofn = {0};
    TCHAR szSaveFile[MAX_PATH] = TEXT("");
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szSaveFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = TEXT("Bitmap Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn))
    {
        char saveFilename[1000];
        #ifdef UNICODE
                        wcstombs(saveFilename, szSaveFile, sizeof(saveFilename));
        #else
                        strncpy(saveFilename, szSaveFile, sizeof(saveFilename));
                        saveFilename[sizeof(saveFilename) - 1] = '\0';
        #endif

        SaveWindowAsBMP(hwnd, saveFilename); // Save the window as BMP
        MessageBox(hwnd, TEXT("Image saved successfully!"), TEXT("Save"), MB_OK | MB_ICONINFORMATION);
    }
}


void SaveWindowAsBMP(HWND hwnd, const char* filename)
{
    int width = 800, height = 600;
    HDC hdcWindow = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdcWindow);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
    SelectObject(hdcMem, hBitmap);

    // Copy drawing from your source memDC to hdcMem
    BitBlt(hdcMem, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

    // Prepare headers
    BITMAPFILEHEADER bmfHeader = {0};
    BITMAPINFOHEADER bi = {0};
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = ((bmp.bmWidth * 3 + 3) & ~3) * bmp.bmHeight;

    DWORD dwBmpSize = bi.biSizeImage;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpbitmap = (char*)GlobalLock(hDIB);

    GetDIBits(hdcMem, hBitmap, 0, (UINT)bmp.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, TEXT("Failed to create file"), TEXT("Error"), MB_OK);
        return;
    }

    DWORD dwBytesWritten = 0;
    bmfHeader.bfType = 0x4D42; // 'BM'
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    WriteFile(hFile, &bmfHeader, sizeof(bmfHeader), &dwBytesWritten, NULL);
    WriteFile(hFile, &bi, sizeof(bi), &dwBytesWritten, NULL);
    WriteFile(hFile, lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    CloseHandle(hFile);
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdcWindow);
}

void ShowSaveFileDialog(HWND hwnd) {
    OPENFILENAME ofn = {0};
    TCHAR szSaveFile[MAX_PATH] = TEXT("");
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szSaveFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = TEXT("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) {
        char saveFilename[1000];
        #ifdef UNICODE
                        wcstombs(saveFilename, szSaveFile, sizeof(saveFilename));
        #else
                        strncpy(saveFilename, szSaveFile, sizeof(saveFilename));
                        saveFilename[sizeof(saveFilename) - 1] = '\0';
        #endif

        SaveLogToFile(saveFilename); // Save the log to the specified file
        MessageBox(hwnd, TEXT("Log saved successfully!"), TEXT("Save"), MB_OK | MB_ICONINFORMATION);
    }
}

void SaveLogToFile(const char* filename) {
    std::ofstream out(filename);
    for (const auto& line : actionLog) {
        out << line << "\n";
    }
    out.close();
}

#endif // WINDEV_GRAPH_PAPER2_MENU_HPP