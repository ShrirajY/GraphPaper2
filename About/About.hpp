#include <windows.h>
#include <tchar.h>

#define IDT_SHAPE_ANIM 1001

enum SHAPE_KIND
{
    SHAPE_TRIANGLE = 0,
    SHAPE_CIRCLE,
    SHAPE_PENTAGON,
    SHAPE_COUNT
};

static int g_shapeIndex = 0;
static int g_colorIndex = 0;
HICON hIconAbout = NULL;
static const COLORREF g_shapeColors[] = {
    RGB(255, 0, 0),    // red
    RGB(0, 255, 0),    // green
    RGB(0, 128, 255),  // blue-ish
    RGB(255, 255, 0),  // yellow
    RGB(255, 0, 255),  // magenta
    RGB(0, 255, 255),  // cyan
    RGB(255, 255, 255) // white
};
static const int g_shapeColorCount = sizeof(g_shapeColors) / sizeof(g_shapeColors[0]);

#include <math.h> // for sin/cos

static void DrawAnimatedShape(HDC hdc, const RECT *prcClient, int shapeIndex)
{
    const int size = 64;    // bounding box width/height
    const int padding = 10; // distance from top/right

    RECT r;
    r.right = prcClient->right - padding;
    r.left = r.right - size;
    r.top = padding;
    r.bottom = r.top + size;

    // Assign one color for each shape
    COLORREF color;
    switch (shapeIndex)
    {
    case SHAPE_TRIANGLE:
        color = RGB(255, 0, 0);
        break; // Red
    case SHAPE_CIRCLE:
        color = RGB(0, 255, 0);
        break; // Green
    case SHAPE_PENTAGON:
        color = RGB(0, 128, 255);
        break; // Blue
    default:
        color = RGB(255, 255, 255);
        break;
    }

    // Use pen only (no fill)
    HPEN hPen = CreatePen(PS_SOLID, 3, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    switch (shapeIndex)
    {
    case SHAPE_TRIANGLE:
    {
        POINT pts[3];
        int cx = (r.left + r.right) / 2; // Center X
        int cy = (r.top + r.bottom) / 2; // Center Y
        int rad = size / 2 - 2;

        // Generate equilateral triangle vertices
        for (int i = 0; i < 3; i++)
        {
            double ang = M_PI / 2 + i * (2 * M_PI / 3); // Start from top, rotate clockwise
            pts[i].x = cx + (int)(rad * cos(ang));
            pts[i].y = cy - (int)(rad * sin(ang)); // subtract for proper screen Y-axis
        }

        Polygon(hdc, pts, 3);
    }

    break;

    case SHAPE_CIRCLE:
        Ellipse(hdc, r.left, r.top, r.right, r.bottom);
        break;

    case SHAPE_PENTAGON:
    default:
    {
        POINT pts[5];
        int cx = (r.left + r.right) / 2;
        int cy = (r.top + r.bottom) / 2;
        int rad = size / 2 - 2;
        for (int i = 0; i < 5; i++)
        {
            double ang = -M_PI / 2 + i * (2 * M_PI / 5);
            pts[i].x = cx + (int)(rad * cos(ang));
            pts[i].y = cy + (int)(rad * sin(ang));
        }
        Polygon(hdc, pts, 5);
    }
    break;
    }

    // Restore old objects
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

int flagDialogAbout = 0;
WNDCLASSEX wcDialogAbout;
LRESULT CALLBACK DialogProcAboutMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateDialogWindowAboutMenu(HWND hwndOwner, HINSTANCE hInst)
{
    if (flagDialogAbout == 0)
    {
        wcDialogAbout.cbSize = sizeof(WNDCLASSEX);
        wcDialogAbout.style = CS_HREDRAW | CS_VREDRAW;
        wcDialogAbout.lpfnWndProc = DialogProcAboutMenu;
        wcDialogAbout.cbClsExtra = 0;
        wcDialogAbout.cbWndExtra = 0;
        wcDialogAbout.hInstance = hInst;
        wcDialogAbout.hIcon = LoadIcon(wcDialogAbout.hInstance, MAKEINTRESOURCE(101));
        wcDialogAbout.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcDialogAbout.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcDialogAbout.lpszMenuName = NULL;
        wcDialogAbout.lpszClassName = TEXT("DialogWindowClassAbout");
        wcDialogAbout.hIconSm = LoadIcon(wcDialogAbout.hInstance, MAKEINTRESOURCE(101));
        wcDialogAbout.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
        if (!RegisterClassEx(&wcDialogAbout))
        {
            MessageBox(NULL, TEXT("Failed to register dialog window class"), TEXT("Error"), MB_OK | MB_ICONERROR);
            return;
        }
        flagDialogAbout = 1;
    }

    int windowWidth = 600;
    int windowHeight = 400;

    // Get the screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the position to center the window on the screen
    int xPos = (screenWidth - windowWidth) / 2;
    int yPos = (screenHeight - windowHeight) / 2;

    // Create the window with the calculated position and size
    HWND hwndDialogAbout = CreateWindowEx(
        0,                              // Extended style
        TEXT("DialogWindowClassAbout"), // Window class
        TEXT("About Dialog"),           // Window title
        WS_OVERLAPPEDWINDOW,            // Style
        xPos,                           // X position (centered)
        yPos,                           // Y position (centered)
        windowWidth,                    // Window width
        windowHeight,                   // Window height
        NULL,                           // Parent window (no parent)
        NULL,                           // Menu
        hInst,                          // Instance handle
        NULL                            // Additional data
    );

    if (hwndDialogAbout == NULL)
    {
        MessageBox(NULL, TEXT("Window creation failed!"), TEXT("Error"), MB_OK | MB_ICONERROR);
    }
    else
    {
        ShowWindow(hwndDialogAbout, SW_SHOWNORMAL);
        UpdateWindow(hwndDialogAbout);
    }
}

HWND hwndDialogAbout = NULL;
// Dialog Procedure
LRESULT CALLBACK DialogProcAboutMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        hIconAbout = (HICON)LoadImage(NULL, TEXT("D:\\WINDEV-2025\\Project\\GraphPaper2\\Logo.ico"),
    IMAGE_ICON, 1600, 1600, LR_LOADFROMFILE);
        // *** NEW *** start animation timer (change every 700 ms; tweak as desired)
        SetTimer(hwnd, IDT_SHAPE_ANIM, 1500, NULL);
        return TRUE;

    case WM_TIMER:
        if (wParam == IDT_SHAPE_ANIM)
        {
            g_shapeIndex = (g_shapeIndex + 1) % SHAPE_COUNT;

            RECT rcClient;
            GetClientRect(hwnd, &rcClient);

            RECT r;
            const int size = 64;
            const int padding = 10;
            r.right = rcClient.right - padding;
            r.left = r.right - size;
            r.top = padding;
            r.bottom = r.top + size;

            InvalidateRect(hwnd, &r, TRUE);
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        

        // Fill background
        HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 60));
        // FillRect(hdc, &clientRect, bgBrush);
        DeleteObject(bgBrush);

        // Arcs
        HPEN arcPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, arcPen);
        Arc(hdc, 20, 20, clientRect.right - 20, clientRect.bottom - 20, 0, 0, 0, 0);
        Arc(hdc, 50, 50, clientRect.right - 50, clientRect.bottom - 50, 0, 0, 0, 0);
        SelectObject(hdc, oldPen);
        DeleteObject(arcPen);

        // *** NEW *** animated shape in top-right
        DrawAnimatedShape(hdc, &clientRect, g_shapeIndex);

        // Font
        LOGFONT lf = {0};
        lf.lfHeight = 22;
        lf.lfWeight = FW_BOLD;
        lf.lfItalic = FALSE;
        lstrcpy(lf.lfFaceName, TEXT("Georgia"));
        HFONT hFont = CreateFontIndirect(&lf);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        SetBkMode(hdc, TRANSPARENT);

        // Title (moved further up: -100)
        RECT textRect;
        SetRect(&textRect, 0, (clientRect.bottom / 4) - 100, clientRect.right, clientRect.bottom / 2);

        SetTextColor(hdc, RGB(0, 0, 0));
        OffsetRect(&textRect, 2, 2);
        DrawText(hdc, TEXT("PixelLogic"), -1, &textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        OffsetRect(&textRect, -2, -2);
        SetTextColor(hdc, RGB(0, 255, 128));
        DrawText(hdc, TEXT("PixelLogic"), -1, &textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

        // Subtitle
        RECT subtitleRect = {0, (clientRect.bottom / 2) - 60, clientRect.right, (clientRect.bottom / 2) + 30};
        SetTextColor(hdc, RGB(0, 255, 255));
        DrawText(hdc, TEXT("Logic Meets Pixels, Art Becomes Habit"), -1, &subtitleRect, DT_CENTER | DT_SINGLELINE);

        // Created by
        RECT authorRect = {0, (clientRect.bottom / 2), clientRect.right, (clientRect.bottom / 2) + 90};
        SetTextColor(hdc, RGB(255, 215, 0));
        DrawText(hdc, TEXT("Created By: Shriraj Shridhar Yamkanmardi"), -1, &authorRect, DT_CENTER | DT_SINGLELINE);

        // Bottom line
        RECT igniteRect = {0, (clientRect.bottom / 2) + 40, clientRect.right, clientRect.bottom - 10};
        SetTextColor(hdc, RGB(255, 128, 0));
        DrawText(hdc, TEXT("Ignited By: ASTROMEDICOMP"), -1, &igniteRect, DT_CENTER | DT_SINGLELINE);

        if (hIconAbout)
        {
            DrawIconEx(hdc, 2, 2, hIconAbout, 150, 100, 0, NULL, DI_NORMAL); // Custom size
        }
        else
        {
            MessageBox(hwnd, "Failed", "asca", MB_OK);
            exit(0);
        }

        // Cleanup font
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        hwndDialogAbout = NULL;
        if (hIconAbout)
        {
            DestroyIcon(hIconAbout);
        }
        // *** NEW *** stop timer
        KillTimer(hwnd, IDT_SHAPE_ANIM);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
