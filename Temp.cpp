// cube.cpp
// Win32 GDI rotating cube (edge length = 4, vertices +/-2).
// Smooth, time-based rotation; simultaneous keys supported; double-buffered.

#include <windows.h>
#include <stdint.h>
#include <math.h>

// ---------- vertex type ----------
struct Vec3 { double x,y,z; };

// ---------- cube vertices (edge length 4 -> +/-2) ----------
Vec3 vertices[] = {
    // Base triangle (y = -2)
    {-2, -2, -2},  // 0
    { 2, -2, -2},  // 1
    { 0, -2,  2},  // 2

    // Top triangle (y = +2)
    {-2,  2, -2},  // 3
    { 2,  2, -2},  // 4
    { 0,  2,  2}   // 5
};

int edges[][2] = {
    // Base triangle
    {0, 1}, {1, 2}, {2, 0},
    // Top triangle
    {3, 4}, {4, 5}, {5, 3},
    // Vertical edges
    {0, 3}, {1, 4}, {2, 5}
};



// per-edge colors (nice variety)
static COLORREF edgeColors[12] = {
    RGB(255,0,0), RGB(255,128,0), RGB(255,200,0), RGB(200,255,0),
    RGB(0,200,0), RGB(0,255,128), RGB(0,200,255), RGB(0,128,255),
    RGB(128,0,255), RGB(200,0,255), RGB(255,0,128), RGB(255,80,160)
};

// ---------- global state ----------
static double angleX = 0.0;    // radians
static double angleY = 0.0;    // radians
static bool keyLeft=false, keyRight=false, keyUp=false, keyDown=false;

static HDC g_memDC = NULL;
static HBITMAP g_memBM = NULL;
static HBITMAP g_oldBM = NULL;
static HPEN   g_pens[12] = {0};
static HBRUSH g_bgBrush = NULL;

static int g_clientW = 800;
static int g_clientH = 600;

// rotation speed when a key is held (radians per second)
static const double ROT_SPEED = 2.5; // adjust to taste

// camera/projection parameters tuned for larger cube
static const double CAMERA_DIST = 8.0;   // move camera further away for larger cube
static const double BASE_SCALE   = 160.0; // visual size scaling

// ---------- timing helpers ----------
static LARGE_INTEGER g_freq;
static LARGE_INTEGER lastTime;

static double getDeltaSeconds() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double dt = double(now.QuadPart - lastTime.QuadPart) / double(g_freq.QuadPart);
    lastTime = now;
    return dt;
}

// ---------- helper: create/delete pens, buffer ----------
static void createPens() {
    for (int i=0;i<12;i++) {
        if (g_pens[i]) DeleteObject(g_pens[i]);
        g_pens[i] = CreatePen(PS_SOLID, 2, edgeColors[i]);
    }
}

static void deletePens() {
    for (int i=0;i<12;i++) {
        if (g_pens[i]) { DeleteObject(g_pens[i]); g_pens[i] = NULL; }
    }
}

static void createBackBuffer(HWND hwnd, int w, int h) {
    if (g_memDC) {
        // restore and delete old
        SelectObject(g_memDC, g_oldBM);
        DeleteObject(g_memBM);
        DeleteDC(g_memDC);
        g_memDC = NULL; g_memBM = NULL; g_oldBM = NULL;
    }
    HDC hdc = GetDC(hwnd);
    g_memDC = CreateCompatibleDC(hdc);
    g_memBM = CreateCompatibleBitmap(hdc, (w>0?w:1), (h>0?h:1));
    g_oldBM = (HBITMAP)SelectObject(g_memDC, g_memBM);
    ReleaseDC(hwnd, hdc);
}

static void freeBackBuffer() {
    if (g_memDC) {
        SelectObject(g_memDC, g_oldBM);
        DeleteObject(g_memBM);
        DeleteDC(g_memDC);
        g_memDC = NULL; g_memBM = NULL; g_oldBM = NULL;
    }
}

// ---------- rotate & project ----------
static Vec3 rotateXY(const Vec3 &p, double ax, double ay) {
    // rotate around X
    double cx = cos(ax), sx = sin(ax);
    double y1 = p.y * cx - p.z * sx;
    double z1 = p.y * sx + p.z * cx;
    double x1 = p.x;
    // rotate around Y
    double cy = cos(ay), sy = sin(ay);
    double z2 = z1 * cy - x1 * sy;
    double x2 = z1 * sy + x1 * cy;
    return { x2, y1, z2 };
}

static POINT projectToScreen(const Vec3 &p, int w, int h) {
    // perspective: camera at +Z looking to -Z
    double denom = (CAMERA_DIST - p.z);
    if (denom < 0.0001) denom = 0.0001;
    double factor = BASE_SCALE / denom;
    int sx = (int)round(w*0.5 + p.x * factor);
    int sy = (int)round(h*0.5 - p.y * factor); // screen Y grows down
    return { sx, sy };
}

// ---------- drawing ----------
static void drawScene(HWND hwnd) {
    if (!g_memDC) return;
    RECT rc; GetClientRect(hwnd,&rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w<=0||h<=0) return;

    // clear background
    FillRect(g_memDC, &rc, g_bgBrush);

    // compute rotated + projected points
    POINT pts[8];
    for (int i=0;i<8;i++) {
        Vec3 r = rotateXY(vertices[i], angleX, angleY);
        pts[i] = projectToScreen(r, w, h);
    }

    // draw edges colored
    for (int i=0;i<12;i++) {
        SelectObject(g_memDC, g_pens[i]);
        MoveToEx(g_memDC, pts[ edges[i][0] ].x, pts[ edges[i][0] ].y, NULL);
        LineTo(  g_memDC, pts[ edges[i][1] ].x, pts[ edges[i][1] ].y);
    }
}

// ---------- window proc ----------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        QueryPerformanceFrequency(&g_freq);
        QueryPerformanceCounter(&lastTime);

        // create pens and bg brush and buffer
        createPens();
        g_bgBrush = CreateSolidBrush(RGB(10,10,20));

        RECT rc; GetClientRect(hwnd,&rc);
        g_clientW = rc.right - rc.left;
        g_clientH = rc.bottom - rc.top;
        createBackBuffer(hwnd, g_clientW, g_clientH);

        // show instructions in title
        SetWindowTextA(hwnd, "Cube (edge=4) — hold arrow keys to rotate; Esc to quit");
        return 0;

    case WM_SIZE:
        g_clientW = LOWORD(lParam);
        g_clientH = HIWORD(lParam);
        createBackBuffer(hwnd, g_clientW, g_clientH);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT)  keyLeft = true;
        if (wParam == VK_RIGHT) keyRight = true;
        if (wParam == VK_UP)    keyUp = true;
        if (wParam == VK_DOWN)  keyDown = true;
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
        return 0;

    case WM_KEYUP:
        if (wParam == VK_LEFT)  keyLeft = false;
        if (wParam == VK_RIGHT) keyRight = false;
        if (wParam == VK_UP)    keyUp = false;
        if (wParam == VK_DOWN)  keyDown = false;
        return 0;

    case WM_DESTROY:
        // cleanup
        freeBackBuffer();
        deletePens();
        if (g_bgBrush) { DeleteObject(g_bgBrush); g_bgBrush = NULL; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

// ---------- entry & main loop ----------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    const char CLASS_NAME[] = "SmoothCubeClass";

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "Smooth Cube - GDI", WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, g_clientW, g_clientH,
                                NULL, NULL, hInst, NULL);
    if (!hwnd) return 0;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // main loop: handle messages + update/draw at ~60Hz
    MSG msg;
    bool running = true;
    const double targetFrame = 1.0 / 60.0; // seconds
    while (running) {
        // process OS messages
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { running = false; break; }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        if (!running) break;

        // time delta
        double dt = getDeltaSeconds();

        // update rotation based on keys (time-based)
        if (keyLeft)  angleY -= ROT_SPEED * dt;
        if (keyRight) angleY += ROT_SPEED * dt;
        if (keyUp)    angleX -= ROT_SPEED * dt;
        if (keyDown)  angleX += ROT_SPEED * dt;

        // draw into back buffer then blit
        drawScene(hwnd);
        HDC hdc = GetDC(hwnd);
        BitBlt(hdc, 0, 0, g_clientW>0?g_clientW:1, g_clientH>0?g_clientH:1, g_memDC, 0, 0, SRCCOPY);
        ReleaseDC(hwnd, hdc);

        // simple frame cap to ~60fps (sleep remainder)
        // measure elapsed since lastTime already set in getDeltaSeconds()
        // do small sleep to avoid 100% CPU
        double elapsed = getDeltaSeconds(); // measure tiny interval
        double wait = targetFrame - elapsed;
        if (wait > 0.001) Sleep((DWORD)(wait * 1000.0));
        // update lastTime for next loop (we call getDeltaSeconds at top)
    }

    return 0;
}
