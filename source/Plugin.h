#ifndef PLUGIN_H
#define PLUGIN_H
#endif

#include <windows.h>
#include <iostream>

#include "Utils.h"
#include <RakHook/rakhook.hpp>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx9.h>

#include <d3d9.h>
// #pragma comment(lib, "d3d9.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern bool imGuiOn;

#define GWL_WNDPROC 	(-4)
#define D3D_VFUNCTIONS 	(119)
#define DEVICE_PTR 		(0xC97C28)
#define ENDSCENE_INDEX 	(42)
#define RESET_INDEX 	(16)

typedef HRESULT(__stdcall* _EndScene)(IDirect3DDevice9* pDevice);
typedef long(__stdcall* _Reset)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pp);

class c_plugin
{
	public:
		c_plugin(HMODULE hmodule);
		~c_plugin();

		static void everything();
		static void attach_console();

		static void game_loop();
		static c_hook<void(*)()> game_loop_hook;
	private:
		HMODULE hmodule;
};
inline c_hook<void(*)()> c_plugin::game_loop_hook = { 0x561B10 };
