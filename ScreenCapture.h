#pragma once

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

class ScreenCapture
{
private:
    HRESULT hr = S_OK;
    IDirect3D9* d3d = nullptr;
    IDirect3DDevice9* device = nullptr;
    IDirect3DSurface9* surface = nullptr;
    D3DPRESENT_PARAMETERS parameters = { 0 };
    D3DLOCKED_RECT rc;
    UINT pitch;
    LPBYTE* shots = nullptr;

public:
    D3DDISPLAYMODE mode;
    void InitCapture();
    void CaptureFrame();
    void Cleanup();
    unsigned char* pixels;
};

