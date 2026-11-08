#include "SDKDDKVer.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "ui_scene.h"
#include "app/ui_layout.h"

namespace
{

ui_scene scene;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void paint(HWND hwnd, bool all = false)
{
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);
    auto changed_rect = scene.draw({ rect.right, rect.bottom });
    if (all) changed_rect = size2i(rect.right, rect.bottom);
    if (!changed_rect.empty())
        BitBlt(hdc, int(changed_rect.x.min), int(changed_rect.y.min), int(changed_rect.x.length()),
            int(changed_rect.y.length()), scene.canvas.hdc, int(changed_rect.x.min), int(changed_rect.y.min),
            SRCCOPY);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool tracking_mouse = false;
    switch (message)
    {
    case WM_CHAR:
        scene.key_press(wParam);
        paint(hWnd);
        return 0;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_F1: break;
        default:
            scene.key_down(wParam);
            paint(hWnd);
        }
        break;
    case WM_MOUSELEAVE:
        tracking_mouse = false;
        break;
    case WM_LBUTTONDOWN:
        scene.mouse_click((short)LOWORD(lParam), (short)HIWORD(lParam));
        paint(hWnd);
        break;
    case WM_LBUTTONUP:
        scene.mouse_release((short)LOWORD(lParam), (short)HIWORD(lParam));
        paint(hWnd);
        break;
    case WM_MOUSEMOVE:
        if (!tracking_mouse)
        {
            tracking_mouse = true;
            TRACKMOUSEEVENT a{};
            a.cbSize = sizeof(a);
            a.dwFlags = TME_LEAVE;
            a.hwndTrack = hWnd;
            TrackMouseEvent(&a);
        }
        scene.mouse_move((short)LOWORD(lParam), (short)HIWORD(lParam), (wParam & MK_LBUTTON) != 0);
        paint(hWnd);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            paint(hWnd, true);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = L"JournalWindowClass";
    wcex.hIconSm = (HICON)LoadImageW(hInstance, MAKEINTRESOURCE(101), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

    return RegisterClassExW(&wcex);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    initialize_ui_scene(scene);

    MyRegisterClass(hInstance);

    HWND hWnd = CreateWindowW(L"JournalWindowClass", L"journal", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return 0;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
