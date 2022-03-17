#include "common.h"
#include "gui.h"

#include <thread>

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE prevInstance, PWSTR args, int commandShow) {
	
	auto WindowInstance = std::make_unique<gui::WINDOW>("ImGui Test", "TestWindowClass");
	auto DeviceInstance = std::make_unique<gui::DEVICE>();
	auto ImGuiHandlerInstance = std::make_unique<gui::IMGUIHANDLER>();
	auto RendererInstance = std::make_unique<gui::RENDERER>();

	while (gui::exit) {
		try {
			gui::g_renderer->BEGINRENDER();
			gui::g_renderer->Render();
			gui::g_renderer->ENDRENDER();
		}
		catch (std::exception &ex) {
			MessageBoxA(gui::window, ex.what(), "INTERNAL_ERROR", 1);
			gui::exit = !gui::exit;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	RendererInstance.reset();
	ImGuiHandlerInstance.reset();
	DeviceInstance.reset();
	WindowInstance.reset();

	return EXIT_SUCCESS;
}