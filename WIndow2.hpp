// FloodFillWin32.cpp
#ifndef WINDOW2_HPP
#define WINDOW2_HPP
#include <windows.h>
#include <thread>
#include <vector>
#include <queue>
#include <set>
#include <windowsx.h>

constexpr int WindowWidth = 800;
constexpr int WindowHeight = 600;

inline int toIndex(int x, int y) {
    return (y + 300) * WindowWidth + (x + 400);
}

bool isInBounds(int x, int y) {
    return x >= -400 && x < 400 && y >= -300 && y < 300;
}

void floodDirection(COLORREF* pixels, int startX, int startY, COLORREF fillColor, COLORREF startColor, const std::set<COLORREF>& stopColors, int dx, int dy) {
    std::queue<POINT> q;
    q.push({startX, startY});

    while (!q.empty()) {
        POINT p = q.front(); q.pop();
        int x = p.x;
        int y = p.y;

        if (!isInBounds(x, y)) continue;

        int idx = toIndex(x, y);
        COLORREF current = pixels[idx];

        if (current != startColor || fillColor == current)
            continue;

        pixels[idx] = fillColor;

        q.push({x + dx, y});
        q.push({x, y + dy});
    }
}

void FloodFillMultithreaded(COLORREF* pixels, int startX, int startY, COLORREF fillColor, const std::set<COLORREF>& stopColors) {
    if (!isInBounds(startX, startY)) return;

    int startIdx = toIndex(startX, startY);
    COLORREF startColor = pixels[startIdx];

    if (fillColor == startColor || stopColors.find(startColor) == stopColors.end())
        return;

    pixels[startIdx] = fillColor;

    int dx[] = {1, -1, 0, 0, 1, -1, 1, -1};
    int dy[] = {0, 0, 1, -1, 1, -1, -1, 1};

    std::vector<std::thread> threads;

    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(floodDirection, pixels, startX + dx[i], startY + dy[i], fillColor, startColor, std::cref(stopColors), dx[i], dy[i]);
    }

    for (auto& t : threads) {
        t.join();
    }
}

// std::set<COLORREF> BackgroundColors = {
//     RGB(255, 255, 255),
//     RGB(10, 10, 10),
//     RGB(210, 210, 210)
// };

// LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//     static COLORREF* pixels = nullptr;

//     switch (msg) {
//     case WM_CREATE:
//         pixels = new COLORREF[WindowWidth * WindowHeight];
//         for (int y = -300; y < 300; ++y)
//             for (int x = -400; x < 400; ++x)
//                 pixels[toIndex(x, y)] = RGB(255, 255, 255);
//         return 0;

//     case WM_LBUTTONDOWN: {
//         int x = GET_X_LPARAM(lParam) - WindowWidth / 2;
//         int y = GET_Y_LPARAM(lParam) - WindowHeight / 2;
//         FloodFillMultithreaded(pixels, x, y, RGB(255, 0, 0), BackgroundColors);
//         InvalidateRect(hwnd, NULL, FALSE);
//         return 0;
//     }

//     case WM_PAINT: {
//         PAINTSTRUCT ps;
//         HDC hdc = BeginPaint(hwnd, &ps);
//         for (int y = -300; y < 300; ++y) {
//             for (int x = -400; x < 400; ++x) {
//                 SetPixel(hdc, x + WindowWidth / 2, y + WindowHeight / 2, pixels[toIndex(x, y)]);
//             }
//         }
//         EndPaint(hwnd, &ps);
//         return 0;
//     }

//     case WM_DESTROY:
//         delete[] pixels;
//         PostQuitMessage(0);
//         return 0;
//     }
//     return DefWindowProc(hwnd, msg, wParam, lParam);
// }

// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
//     const char CLASS_NAME[] = "FloodFillWindow";

//     WNDCLASS wc = {};
//     wc.lpfnWndProc = WndProc;
//     wc.hInstance = hInstance;
//     wc.lpszClassName = CLASS_NAME;
//     wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

//     RegisterClass(&wc);

//     HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Multithreaded Flood Fill", WS_OVERLAPPEDWINDOW,
//         CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth + 16, WindowHeight + 39,
//         NULL, NULL, hInstance, NULL);

//     ShowWindow(hwnd, nCmdShow);

//     MSG msg = {};
//     while (GetMessage(&msg, NULL, 0, 0)) {
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//     }
//     return 0;
// }

#endif // WINDOW2_HPP