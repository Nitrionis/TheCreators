#pragma once
#include <windows.h>
#include <string>
#include <cassert>

namespace vk
{
    class Window {
    public:
        uint32_t width, screenWidth;
        uint32_t height, screenHeight;

        Window(){}
        ~Window(){ DestroyWindow(); }

#ifdef _WIN32
    public:
        HINSTANCE hinstance;
        HWND hwnd;

        void MsgLoop();

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        void Setup(const char *name, bool fullscreen, int width = 800, int height = 600);

        void DestroyWindow();

#endif
    };
}