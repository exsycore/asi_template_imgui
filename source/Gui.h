#ifndef GUI_H
#define GUI_H
#endif

#include <d3d9.h>

namespace PGui {
    void Init(HWND hWnd, IDirect3DDevice9* pDevice);
    void MainGui();
    void Destroy();
}