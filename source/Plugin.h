#pragma once

#include <windows.h>
#include <iostream>

#include "Utils.h"
#include <RakHook/rakhook.hpp>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx9.h>

#include <d3d9.h>
// #pragma comment(lib, "d3d9.lib")

extern bool imgui_init;

#define GWL_WNDPROC (-4)

class c_plugin
{
	public:
		c_plugin(HMODULE hmodule);
		~c_plugin();

		static void everything();
		static void attach_console();

		static void InitializationImgui(void* hwnd, IDirect3DDevice9* device);
		static void ImguiLoop();

		static void game_loop();
		static c_hook<void(*)()> game_loop_hook;
	private:
		HMODULE hmodule;
};
inline c_hook<void(*)()> c_plugin::game_loop_hook = { 0x561B10 };
