#include "Plugin.h"
#include "Gui.h"

#include <sampapi/CChat.h>

#include <RakNet/PacketEnumerations.h>
#include <RakNet/StringCompressor.h>
#include <RakNet/BitStream.h>

namespace samp = sampapi::v03dl;

_EndScene oEndScene;
_Reset oReset = nullptr;

bool g_bwasInitialized = false;
bool wndproc = false;
int hwndd;
bool imGuiOn = false;

DWORD procID;
HANDLE handle;
HWND hWnd;
WNDPROC oriWndProc = NULL;

void c_plugin::everything()
{
	GetWindowThreadProcessId(hWnd, &procID);
	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
}

LRESULT CALLBACK hWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (imGuiOn && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
		return true;
	}
	return CallWindowProc(oriWndProc, hwnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pp)
{
	if (g_bwasInitialized) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		g_bwasInitialized = false;
	}
	return oReset(pDevice, pp);
}

HRESULT __stdcall hkEndScene(IDirect3DDevice9* pDevice)
{
	if (!g_bwasInitialized) {
		D3DDEVICE_CREATION_PARAMETERS d3dcp;
		pDevice->GetCreationParameters(&d3dcp);
		hWnd = d3dcp.hFocusWindow;

		if (hwndd == 0) {
			oriWndProc = (WNDPROC)SetWindowLongPtr(d3dcp.hFocusWindow,
				GWL_WNDPROC, (LONG)(LONG_PTR)hWndProc);
			hwndd++;
		}

		PGui::Init(d3dcp.hFocusWindow, pDevice);
		g_bwasInitialized = true;
	}

	if (imGuiOn) {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		try {
			PGui::MainGui();
		} catch (const std::exception& e) {
			samp::RefChat()->AddMessage(-1, e.what());
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }
	return oEndScene(pDevice);
}

void c_plugin::game_loop()
{
	static bool initialized = false;

	if (initialized || !rakhook::initialize() || samp::RefChat() == nullptr) {
		return game_loop_hook.call_original();
	}

	initialized = true;
	StringCompressor::AddReference();

	samp::RefChat()->AddMessage(-1, "SF-Plugin | Initialize");

	void** vTableDevice = *(void***)(*(DWORD*)DEVICE_PTR);
	VTableHookManager* vmtHooks = new VTableHookManager(vTableDevice, D3D_VFUNCTIONS);

	oEndScene = (_EndScene)vmtHooks->Hook(ENDSCENE_INDEX, (void*)hkEndScene);
	oReset = (_Reset)vmtHooks->Hook(RESET_INDEX, (void*)hkReset);

	everything();
	//InitializationImgui();
	return game_loop_hook.call_original();
}

void c_plugin::attach_console()
{
	if (!AllocConsole())
		return;

	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONOUT$", "w", stderr);
	freopen_s(&f, "CONIN$", "r", stdin);
}

c_plugin::c_plugin(HMODULE hmodule) : hmodule(hmodule)
{
	attach_console();
	game_loop_hook.add(&c_plugin::game_loop);
}

c_plugin::~c_plugin()
{
	PGui::Destroy();
	rakhook::destroy();
}
