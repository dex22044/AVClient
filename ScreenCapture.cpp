#include "ScreenCapture.h"

void ScreenCapture::InitCapture()
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

    pixels = (unsigned char*)malloc(mode.Width * mode.Height * 4);

    parameters.Windowed = TRUE;
    parameters.BackBufferCount = 1;
    parameters.BackBufferHeight = mode.Height;
    parameters.BackBufferWidth = mode.Width;
    parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    parameters.hDeviceWindow = NULL;

    // create device & capture surface
    if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &device) != D3D_OK) {
        d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &device);
    }
    device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr);
}

void ScreenCapture::CaptureFrame()
{
    device->GetFrontBufferData(0, surface);

    surface->LockRect(&rc, NULL, 0);
    memcpy(pixels, rc.pBits, rc.Pitch * mode.Height);
    surface->UnlockRect();
}

void ScreenCapture::Cleanup()
{
    free(surface);
    free(device);
    free(d3d);
}
