#include "Window.h"
#include <chrono>
#include <thread>
#include <cassert>

using namespace InputControl;

InputSharedData& vk::Window::input = InputSharedData::Instance();

#ifdef _WIN32

LRESULT vk::Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    const uint64_t maxMsg = UINT64_MAX;

    switch (uMsg) {
        case WM_LBUTTONDOWN:
            input.mouseCondsCode[1].x = GET_X_LPARAM(lParam);
            input.mouseCondsCode[1].y = GET_Y_LPARAM(lParam);
            input.mouseCondsDown[1].x = GET_X_LPARAM(lParam);
            input.mouseCondsDown[1].y = GET_Y_LPARAM(lParam);
            input.mouseCondsUp[1].x = -1;
            input.mouseCondsUp[1].y = -1;
            break;
        case WM_RBUTTONDOWN:
            input.mouseCondsCode[2].x = GET_X_LPARAM(lParam);
            input.mouseCondsCode[2].y = GET_Y_LPARAM(lParam);
            input.mouseCondsDown[2].x = GET_X_LPARAM(lParam);
            input.mouseCondsDown[2].y = GET_Y_LPARAM(lParam);
            input.mouseCondsUp[2].x = -1;
            input.mouseCondsUp[2].y = -1;
            break;
        case WM_MBUTTONDOWN:
            input.mouseCondsCode[4].x = GET_X_LPARAM(lParam);
            input.mouseCondsCode[4].y = GET_Y_LPARAM(lParam);
            input.mouseCondsDown[4].x = GET_X_LPARAM(lParam);
            input.mouseCondsDown[4].y = GET_Y_LPARAM(lParam);
            input.mouseCondsUp[4].x = -1;
            input.mouseCondsUp[4].y = -1;
            break;

        case WM_LBUTTONUP:
            input.mouseCondsCode[1].x = -1;
            input.mouseCondsCode[1].y = -1;
            input.mouseCondsDown[1].x = -1;
            input.mouseCondsDown[1].y = -1;
            input.mouseCondsUp[1].x = GET_X_LPARAM(lParam);
            input.mouseCondsUp[1].y = GET_Y_LPARAM(lParam);
            break;
        case WM_RBUTTONUP:
            input.mouseCondsCode[2].x = -1;
            input.mouseCondsCode[2].y = -1;
            input.mouseCondsDown[2].x = -1;
            input.mouseCondsDown[2].y = -1;
            input.mouseCondsUp[2].x = GET_X_LPARAM(lParam);
            input.mouseCondsUp[2].y = GET_Y_LPARAM(lParam);
            break;
        case WM_MBUTTONUP:
            input.mouseCondsCode[4].x = -1;
            input.mouseCondsCode[4].y = -1;
            input.mouseCondsDown[4].x = -1;
            input.mouseCondsDown[4].y = -1;
            input.mouseCondsUp[4].x = GET_X_LPARAM(lParam);
            input.mouseCondsUp[4].y = GET_Y_LPARAM(lParam);
            break;

        case WM_KEYDOWN:
            if (input.keyCondsUp[wParam]) {
                input.timeLineDown[wParam]++;
                if (input.timeLineDown[wParam] >= maxMsg)
                    input.timeLineDown[wParam] = 0;
                input.keyCondsCode[wParam] = true;
                input.keyCondsDown[wParam] = true;
            }
            break;
        case WM_KEYUP:
            input.timeLineUp[wParam]++;
            if (input.timeLineUp[wParam] >= maxMsg)
                input.timeLineUp[wParam] = 0;
            input.keyCondsCode[wParam] = false;
            input.keyCondsUp[wParam] = true;
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:

            break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void vk::Window::Setup(const char *name, bool fullscreen, int width, int height) {
    HMONITOR hmon = MonitorFromWindow(NULL, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);

    screenWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    screenHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

    assert(width > 0 && width <= screenWidth);
    assert(height > 0 && height <= screenHeight);

    hinstance = GetModuleHandle(NULL);

    WNDCLASSEX winClass;
    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = WndProc;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = hinstance;
    winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = "VulkanWindow";
    winClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    if (!RegisterClassEx(&winClass))
    {
        MessageBox(NULL, "Error registering class", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (fullscreen)
    {
        width = screenWidth;
        height = screenHeight;
    }

    RECT wr = {0, 0, width, height};
    //AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindowEx(
        0,
        "VulkanWindow",      // class name
        name,                // app name
        WS_POPUP | WS_EX_TOPMOST, // !!!!!!!!!!!!!!!!!!
        //WS_OVERLAPPEDWINDOW |// hwnd style
        //WS_VISIBLE | WS_SYSMENU,
        0, 0,                // x/y coords
        wr.right - wr.left,  // width
        wr.bottom - wr.top,  // height
        NULL,                // handle to parent
        NULL,                // handle to menu
        hinstance,           // hInstance
        NULL                 // no extra parameters
    );

    if (!hwnd) {
        MessageBox(NULL, "Error creating hwnd", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    ShowWindow(hwnd, 1);
    UpdateWindow(hwnd);

    ZeroMemory(&msg, sizeof(MSG));
}

bool vk::Window::CheckMsg() {

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    input.prevPosition = input.mousePosition;
    GetCursorPos(&input.mousePosition);
    input.mouseMove.x = input.mousePosition.x - input.prevPosition.x;
    input.mouseMove.y = input.mousePosition.y - input.prevPosition.y;

	if (input.keyCondsDown[VK_ESCAPE])
		return false;

    return true;
}

void vk::Window::DestroyWindow() {
    UnregisterClass("VulkanWindow", hinstance);
}

#endif