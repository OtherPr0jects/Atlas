#include <iostream>
#include <Windows.h>
#include <d3d9.h>
#include <thread>
#include "Gui.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "Utility.h"
#include "Globals.h"
#include "Render.h"
#include "Aimbot.h"
#include "RBXStructs.h"
#pragma comment(lib, "d3d9.lib")

static LPDIRECT3D9 g_pD3D;
static LPDIRECT3DDEVICE9 g_pd3dDevice;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

bool CreateDeviceD3D(HWND hwnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Gui::Setup() {
    Render render;
    render.Setup();

    Aimbot aimbot;
    std::thread aimbotThread([&]() {
        aimbot.Setup();
    });

    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = (LPCWSTR)Utility::CreateRandomString(6).c_str();
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        wc.lpszClassName,
        (LPCWSTR)Utility::CreateRandomString(6).c_str(),
        WS_OVERLAPPEDWINDOW,
        0, 0,
        500, 500,
        NULL,
        NULL,
        wc.hInstance,
        NULL
    );
    Globals::GuiHWND = hwnd;

    if (!CreateDeviceD3D(hwnd)) {
        std::cout << "Couldn't create D3D device." << std::endl;
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImVec4 crosshairColor = ImVec4(Globals::CrosshairColor.R, Globals::CrosshairColor.G, Globals::CrosshairColor.B, 1);
    ImVec4 espBoxColor = ImVec4(Globals::ESPBoxColor.R, Globals::ESPBoxColor.G, Globals::ESPBoxColor.B, 1);
    ImVec4 nameColor = ImVec4(Globals::NameColor.R, Globals::NameColor.G, Globals::NameColor.B, 1);
    ImVec4 distanceColor = ImVec4(Globals::DistanceColor.R, Globals::DistanceColor.G, Globals::DistanceColor.B, 1);

    const char* targetOptions[] = {
        "Head",
        "HumanoidRootPart",
        "Random"
    };

    const char* aimbotBindOptions[] = {
        "Right Mouse Button",
        "Left Mouse Button",
        "Left Shift"
    };

    bool done = false;
    while (!done) {
        if (!FindWindowA(0, "Roblox")) {
            exit(0);
        }

        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) {
            break;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RECT rect;
        float width = 0;
        float height = 0;
        if (GetWindowRect(hwnd, &rect)) {
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ width, height });

        {
            ImGui::Begin("Atlas", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

            ImGui::Checkbox("ESP", &Globals::ESPEnabled);
            ImGui::Checkbox("Aimbot", &Globals::AimbotEnabled);
            ImGui::Checkbox("Team check", &Globals::TeamCheck);
            ImGui::Checkbox("Crosshair", &Globals::Crosshair);
            ImGui::ListBox("Target", &Globals::Target, targetOptions, IM_ARRAYSIZE(targetOptions));
            ImGui::ListBox("Aimbot bind", &Globals::AimbotBind, aimbotBindOptions, IM_ARRAYSIZE(aimbotBindOptions));
            ImGui::SliderFloat("Crosshair scale", &Globals::CrosshairScale, 0.1, 5);
            ImGui::SliderFloat("Crosshair thickness", &Globals::CrosshairThickness, 1, 6);
            ImGui::SliderFloat("FOV size", &Globals::FOVSize, 0, 1000);
            ImGui::SliderFloat("Aimbot smoothness", &Globals::AimbotSmoothness, 4, 15);

            ImGui::ColorEdit3("Crosshair color", (float*)&crosshairColor);
            Globals::CrosshairColor = { crosshairColor.x, crosshairColor.y, crosshairColor.z };

            ImGui::ColorEdit3("ESP box color", (float*)&espBoxColor);
            Globals::ESPBoxColor = { espBoxColor.x, espBoxColor.y, espBoxColor.z };

            ImGui::ColorEdit3("Name color", (float*)&nameColor);
            Globals::NameColor = { nameColor.x, nameColor.y, nameColor.z };

            ImGui::ColorEdit3("Distance color", (float*)&distanceColor);
            Globals::DistanceColor = { distanceColor.x, distanceColor.y, distanceColor.z };

            ImGui::End();
        }

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool CreateDeviceD3D(HWND hwnd) {
    if (!(g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D() {
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}