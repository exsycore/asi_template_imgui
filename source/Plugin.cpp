#include "Plugin.h"

#include <RakNet/PacketEnumerations.h>
#include <RakNet/StringCompressor.h>
#include <RakNet/BitStream.h>

#include <samp.h>

bool imgui_init = false;

#define D3D_VFUNCTIONS 119
#define DEVICE_PTR 0xC97C28
#define ENDSCENE_INDEX 42
#define RESET_INDEX 16

typedef HRESULT(__stdcall* _EndScene)(IDirect3DDevice9* pDevice);
_EndScene oEndScene;

typedef long(__stdcall* _Reset)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pp);
_Reset oReset = nullptr;

bool g_bwasInitialized = false;
bool wndproc = false;
int hwndd;

DWORD key;
DWORD procID;
HANDLE handle;
HWND hWnd;

WNDPROC oriWndProc = NULL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

template <typename T>
std::string read_with_size(RakNet::BitStream *bs) {
    T size;
    if (!bs->Read(size))
        return {};
    std::string str(size, '\0');
    bs->Read(str.data(), size);
    return str;
}

template <typename T>
void write_with_size(RakNet::BitStream *bs, std::string_view str) {
    T size = static_cast<T>(str.size());
    bs->Write(size);
    bs->Write(str.data(), size);
}

void c_plugin::everything()
{
	GetWindowThreadProcessId(hWnd, &procID);
	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
}

LRESULT CALLBACK hWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) && GetKeyState(key) == 1 && wndproc)
	{
		return 1l;
	}

	return CallWindowProc(oriWndProc, hwnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pp)
{
	if (g_bwasInitialized)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		g_bwasInitialized = false;
	}
	return oReset(pDevice, pp);
}

HRESULT __stdcall hkEndScene(IDirect3DDevice9* pDevice)
{
	if (!g_bwasInitialized)
	{
		D3DDEVICE_CREATION_PARAMETERS d3dcp;
		pDevice->GetCreationParameters(&d3dcp);
		hWnd = d3dcp.hFocusWindow;

		if (hwndd == 0)
		{
			oriWndProc = (WNDPROC)SetWindowLongPtr(d3dcp.hFocusWindow,
				GWL_WNDPROC, (LONG)(LONG_PTR)hWndProc);
			hwndd++;
		}

		c_plugin::InitializationImgui(d3dcp.hFocusWindow, pDevice);
		g_bwasInitialized = true;
	}

	c_plugin::ImguiLoop();
	return oEndScene(pDevice);
}

void c_plugin::game_loop()
{
	static bool initialized = false;

	if (initialized || !rakhook::initialize() || c_chat::get()->ref() == nullptr) {
		//ImguiLoop();
		return game_loop_hook.call_original();
	}

	initialized = true;
	StringCompressor::AddReference();

	c_chat::get()->ref()->add_message(-1, "SF-Plugin | Initialize");

	void** vTableDevice = *(void***)(*(DWORD*)DEVICE_PTR);
	VTableHookManager* vmtHooks = new VTableHookManager(vTableDevice, D3D_VFUNCTIONS);

	oEndScene = (_EndScene)vmtHooks->Hook(ENDSCENE_INDEX, (void*)hkEndScene);
	oReset = (_Reset)vmtHooks->Hook(RESET_INDEX, (void*)hkReset);

	everything();
	//InitializationImgui();

	rakhook::on_receive_rpc += [](unsigned char& id, RakNet::BitStream* bs) -> bool
	{
		if (id == 61) // onShowDialog
		{
			printf("dialog\n");
			unsigned short did;
			unsigned char style;
			std::string title, but1, but2, text(4096, 0);

			// read
			bs->Read(did);
			bs->Read(style);
			title = read_with_size<unsigned char>(bs);
			but1 = read_with_size<unsigned char>(bs);
			but2 = read_with_size<unsigned char>(bs);
			StringCompressor::Instance()->DecodeString(text.data(), 4096, bs);

			title = std::to_string(id) + " | " + title;
			text = "[HOOKED] " + text;
			size_t pos = text.find('\0');
			if (pos != std::string::npos)
				text.insert(pos, " [HOOKED]");
			text.resize(4096);

			// write
			bs->Reset();
			bs->Write(did);
			bs->Write(style);
			
			write_with_size<unsigned char>(bs, title);
			write_with_size<unsigned char>(bs, but1);
			write_with_size<unsigned char>(bs, but2);
			StringCompressor::Instance()->EncodeString(text.data(), 4096, bs);
		}
		return true;
	};

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

void c_plugin::InitializationImgui(void* hwnd, IDirect3DDevice9* device)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(device);
	imgui_init = true;
}

void c_plugin::ImguiLoop()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

c_plugin::c_plugin(HMODULE hmodule) : hmodule(hmodule)
{
	attach_console();
	game_loop_hook.add(&c_plugin::game_loop);
}

c_plugin::~c_plugin()
{
	if (imgui_init)
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	rakhook::destroy();
}
