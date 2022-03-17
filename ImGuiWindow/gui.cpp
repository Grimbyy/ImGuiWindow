#include "gui.h"

#include <iostream>
#include <string>
#include <filesystem>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg) {
	case WM_SYSCOMMAND: {
		if ((wParam & 0xfff0) == SC_KEYMENU) {
			return 0;
		}
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(lParam);
	}return 0;
	
	case WM_MOUSEMOVE: {
		if (wParam == MK_LBUTTON) {
			const auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT{};

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
			{
				SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
			}
		}
	}return 0;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

namespace gui {

	WINDOW::WINDOW(const char* windowName, const char* className) noexcept 
	{
		windowClass.cbSize = sizeof(WNDCLASSEXA);
		windowClass.style = CS_CLASSDC;
		windowClass.lpfnWndProc = WndProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = GetModuleHandleA(0);
		windowClass.hIcon = 0;
		windowClass.hCursor = 0;
		windowClass.hbrBackground = 0;
		windowClass.lpszMenuName = 0;
		windowClass.lpszClassName = className;
		windowClass.hIconSm = 0;

		RegisterClassExA(&windowClass);

		window = CreateWindowA(className, windowName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

		ShowWindow(window, SW_SHOWDEFAULT);
		UpdateWindow(window);

		g_window = this;
	}

	WINDOW::~WINDOW() noexcept
	{
		DestroyWindow(window);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

		g_window = nullptr;
	}

	DEVICE::DEVICE() noexcept 
	{
		d3d = Direct3DCreate9(D3D_SDK_VERSION);

		if (!d3d) return;

		ZeroMemory(&presentParameters, sizeof(presentParameters));

		presentParameters.Windowed = true;
		presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
		presentParameters.EnableAutoDepthStencil = TRUE;
		presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
		presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		if (d3d->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&presentParameters,
			&device
		) < 0) return;

		g_device = this;
	}

	DEVICE::~DEVICE() noexcept
	{
		if (g_device != nullptr) {
			if (device) {
				device->Release();
				device = nullptr;
			}

			if (d3d) {
				d3d->Release();
				d3d = nullptr;
			}

			g_device = nullptr;
		}
	}

	void DEVICE::RESET() noexcept
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();

		const auto result = device->Reset(&presentParameters);

		if (result == D3DERR_INVALIDCALL) {
			IM_ASSERT(0);
		}

		ImGui_ImplDX9_CreateDeviceObjects();
	}

	IMGUIHANDLER::IMGUIHANDLER() noexcept 
	{
		ImGui::CreateContext();
		ImGuiIO& io = ::ImGui::GetIO();

		io.IniFilename = NULL;

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(device);

		g_imguihandler = this;
	}

	IMGUIHANDLER::~IMGUIHANDLER() noexcept
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		g_imguihandler = nullptr;
	}

	RENDERER::RENDERER() noexcept 
	{
		g_renderer = this;
	}
	RENDERER::~RENDERER() noexcept
	{
		g_renderer = nullptr;
	}

	void RENDERER::BEGINRENDER() noexcept
	{
		MSG message;
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//Start ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void RENDERER::ENDRENDER() noexcept
	{
		ImGui::EndFrame();

		device->SetRenderState(D3DRS_ZENABLE, FALSE);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

		if (device->BeginScene() >= 0) {
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			device->EndScene();
		}

		const auto result = device->Present(0, 0, 0, 0);

		if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			g_device->RESET();

	}

	void RENDERER::Render() noexcept {
		using namespace ImGui;

		SetNextWindowPos({ 0.0f, 0.0f });
		SetNextWindowSize({ WIDTH, HEIGHT });
		if (Begin("Basic ImGUI DirectX 9 Window", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {
			

			End();
		}
	}
}