#include "Gui.h"
#include "Plugin.h"

#include <string>
#include <sampapi/CGame.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx9.h>

namespace samp = sampapi::v03dl;

bool* pImguiOn = &imGuiOn;
int imguiInit = false;

void PGui::Init(HWND hWnd, IDirect3DDevice9* pDevice)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(pDevice);

    std::string fontPath2 = "C:\\Windows\\Fonts\\tahoma.ttf";

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesThai());
    builder.BuildRanges(&ranges);

    ImFont* thaifont;
    thaifont = io.Fonts->AddFontFromFileTTF(fontPath2.c_str(), 16.0f, nullptr, ranges.Data);
    io.Fonts->Build();
    io.FontDefault = thaifont;

    imguiInit = true;
    *pImguiOn = true;
}

void PGui::MainGui()
{
    if (!imguiInit)
        throw std::runtime_error("ImGui not initialized");
    
    samp::RefGame()->SetCursorMode(samp::CURSOR_LOCKCAMANDCONTROL, true);
    ImGui::ShowDemoWindow();
}

void PGui::Destroy()
{
    if (imguiInit) {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        
        *pImguiOn = false;
        imguiInit = false;
    }
}