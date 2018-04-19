#pragma once

#include <string>
#include <cassert>
#include "Vulkan/Input/InputControl.h"

#pragma once

namespace vk
{
    class Window {
    private:
        static InputControl::InputSharedData& input;
    public:
        uint32_t width, screenWidth;
        uint32_t height, screenHeight;

        Window(){}
        ~Window(){ DestroyWindow(); }

#ifdef _WIN32
    private:
        MSG msg;

    public:
        HINSTANCE hinstance;
        HWND hwnd;

        bool CheckMsg();

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        void Setup(const char *name, bool fullscreen, int width = 800, int height = 600);

        void DestroyWindow();
#endif
    };
}