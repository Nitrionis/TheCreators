#include "Window.h"

#ifdef _WIN32

LRESULT vk::Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_PAINT:
            break;
        default:
            break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void vk::Window::Setup(const char *name, bool fullscreen, int width, int height)
{
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
        NULL,
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

    if (fullscreen)
    {
        ///GetWindowPlacement(hwnd,&wpc);//Сохраняем параметры оконного режима
        ///SetWindowLong(hwnd,GWL_STYLE,WS_POPUP);//Устанавливаем новые стили
        ///SetWindowLong(hwnd,GWL_EXSTYLE,WS_EX_TOPMOST);
        ///ShowWindow(hwnd,SW_SHOWMAXIMIZED);//Окно во весь экран
        //SetWindowLong(hwnd, GWL_STYLE, 0);
    }

    ShowWindow(hwnd, 1);
    UpdateWindow(hwnd);
}

void vk::Window::MsgLoop() {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            Sleep(100);
            // run game code
        }
    }
}

void vk::Window::DestroyWindow()
{
    UnregisterClass("VulkanWindow", hinstance);
}

#endif