#pragma once

#include <iostream>
#include <stdint.h>

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#endif

namespace vk
{
	class Window;
}

namespace InputControl
{
	enum class KeyCode : uint32_t {
		Zero    = 0x30,
		One     = 0x31,
		Two     = 0x32,
		Three   = 0x33,
		Four    = 0x34,
		Five    = 0x35,
		Six     = 0x36,
		Seven   = 0x37,
		Eight   = 0x38,
		Nine    = 0x39,
		A = 0x41, a = 0x41,
		B = 0x42, b = 0x42,
		C = 0x43, c = 0x43,
		D = 0x44, d = 0x44,
		E = 0x45, e = 0x45,
		F = 0x46, f = 0x46,
		G = 0x47, g = 0x47,
		H = 0x48, h = 0x48,
		I = 0x49, i = 0x49,
		J = 0x4A, j = 0x4A,
		K = 0x4B, k = 0x4B,
		L = 0x4C, l = 0x4C,
		M = 0x4D, m = 0x4D,
		N = 0x4E, n = 0x4E,
		O = 0x4F, o = 0x4F,
		P = 0x50, p = 0x50,
		Q = 0x51, q = 0x51,
		R = 0x52, r = 0x52,
		S = 0x53, s = 0x53,
		T = 0x54, t = 0x54,
		U = 0x55, u = 0x55,
		V = 0x56, v = 0x56,
		W = 0x57, w = 0x57,
		X = 0x58, x = 0x58,
		Y = 0x59, y = 0x59,
		Z = 0x5A, z = 0x5A
	};

	enum class MouseCode : uint32_t {
		left = 1,
		Middle = 2,
		Right = 4
	};

	class InputData {
	public:
		POINT mouseMove;
		POINT mousePosition;
		POINT prevPosition;

		POINT mouseCondsDown[6];
		POINT mouseCondsUp[6];

		uint64_t timeLineDown[256];
		uint64_t timeLineUp[256];

		bool keyCondsDown[256];
		bool keyCondsUp[256];

		InputData();
	};

	class InputInterface;

	class InputSharedData : private InputData
	{
		friend vk::Window;
		friend InputInterface;
	private:
		POINT mouseCondsCode[6];
		bool keyCondsCode[256];

		InputSharedData();

	public:
		~InputSharedData() {}

		static InputSharedData& Instance() {
			static InputSharedData s;
			return s;
		}
	};

	class InputInterface : private InputData
	{
	private:
		static InputSharedData& sharedData;

	public:
		InputInterface(){};

		POINT GetMouseMove();
		POINT GetMousePosition();

		POINT GetMouseDown(MouseCode mouseCode);
		POINT GetMouseCode(MouseCode mouseCode);
		POINT GetMouseUp(MouseCode mouseCode);

		bool GetKeyDown(KeyCode keyCode);
		bool GetKeyCode(KeyCode keyCode);
		bool GetKeyUp(KeyCode keyCode);
	};
}