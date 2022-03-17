#pragma once
#include "common.h"
#include <filesystem>
#include <iostream>
#include <string>

namespace gui {
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 300;

	inline bool exit{ true };

	// WinAPI Window Vars
	inline HWND window{ nullptr };
	inline WNDCLASSEXA windowClass{};

	//Points for window movement
	inline POINTS position{};

	//DX State vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters;

	class WINDOW;
	inline WINDOW* g_window;

	class DEVICE;
	inline DEVICE* g_device;

	class IMGUIHANDLER;
	inline IMGUIHANDLER* g_imguihandler;

	class RENDERER;
	inline RENDERER* g_renderer;

	class WINDOW {
	public:
		WINDOW(const char* windowName, const char* className) noexcept;
		~WINDOW() noexcept;
	};

	class DEVICE {
	public:
		DEVICE() noexcept;
		~DEVICE() noexcept;
		void RESET() noexcept;
	};

	class IMGUIHANDLER {
	public:
		IMGUIHANDLER() noexcept;
		~IMGUIHANDLER() noexcept;
	};

	class RENDERER {
	public:
		RENDERER() noexcept;
		~RENDERER() noexcept;
		void BEGINRENDER() noexcept;
		void ENDRENDER() noexcept;
		void Render() noexcept;
	};
}
