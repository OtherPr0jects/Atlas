#include <iostream>
#include <Windows.h>
#include <d3d9.h>
#include <thread>
#include <map>
#include "Gui.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "Utility.h"
#include "Globals.h"
#include "Render.h"
#include "Aimbot.h"
#include "Config.h"
#pragma comment(lib, "d3d9.lib")

constexpr int MAX_RAINBOW_ITERATION = 230;

float rainbowIteration = 1;

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
        500, 528,
        NULL,
        NULL,
        wc.hInstance,
        NULL
    );
    Globals::GuiHWND = hwnd;

    if (!CreateDeviceD3D(hwnd)) {
        std::cout << "Couldn't create D3D device.\n";
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

    std::map<const char*, std::pair<bool, float*>> rainbowColors;

    auto colorEdit4WithRainbow = [&](const char* label, float* col, ImGuiColorEditFlags flags = 0) {
        if (rainbowColors[label].first) {
            rainbowColors[label].second = col;
        } else {
            rainbowColors[label].second = NULL;
            ImGui::ColorEdit4(label, col);
        }
        ImGui::Checkbox(("Rainbow " + static_cast<std::string>(label)).c_str(), &rainbowColors[label].first);
    };

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

    char configName[25] = {};

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

        rainbowIteration = (rainbowIteration >= MAX_RAINBOW_ITERATION) ? 1 : (rainbowIteration + Globals::RainbowSpeed);

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
            ImGui::BeginTabBar("Tabs");

            if (ImGui::BeginTabItem("ESP")) {
                ImGui::Checkbox("ESP boxes", &Globals::ESPBoxEnabled);
                ImGui::Checkbox("Tracers", &Globals::ESPTracerEnabled);
                if (Globals::ESPTracerEnabled) {
                    ImGui::SameLine();
                    ImGui::RadioButton("From bottom center", &Globals::TracerStartLocation, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("From cursor", &Globals::TracerStartLocation, 1);
                }
                ImGui::Checkbox("Names", &Globals::ESPNameEnabled);
                ImGui::Checkbox("Distances", &Globals::ESPDistanceEnabled);
                ImGui::Checkbox("Head dots", &Globals::ESPHeadDotEnabled);
                ImGui::Checkbox("Health bars", &Globals::HealthBarEnabled);

                ImGui::SliderFloat("ESP box thickness", &Globals::ESPBoxThickness, 1, 6);
                ImGui::SliderFloat("Tracer thickness", &Globals::TracerThickness, 1, 6);
                ImGui::SliderFloat("Health bar thickness", &Globals::HealthBarThickness, 1, 25);

                colorEdit4WithRainbow("ESP box color", reinterpret_cast<float*>(&Globals::ESPBoxColor));
                colorEdit4WithRainbow("Tracer color", reinterpret_cast<float*>(&Globals::TracerColor));
                colorEdit4WithRainbow("Name color", reinterpret_cast<float*>(&Globals::NameColor));
                colorEdit4WithRainbow("Distance color", reinterpret_cast<float*>(&Globals::DistanceColor));
                colorEdit4WithRainbow("Head dot color", reinterpret_cast<float*>(&Globals::HeadDotColor));

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Aimbot")) {
                ImGui::Checkbox("Aimbot", &Globals::AimbotEnabled);
                ImGui::Checkbox("View FOV circle", &Globals::ViewFOVCircle);

                ImGui::Combo("Target", &Globals::Target, targetOptions, IM_ARRAYSIZE(targetOptions));
                ImGui::Combo("Aimbot bind", &Globals::AimbotBind, aimbotBindOptions, IM_ARRAYSIZE(aimbotBindOptions));

                ImGui::SliderFloat("FOV size", &Globals::FOVSize, 0, 1000);
                ImGui::SliderFloat("FOV circle thickness", &Globals::FOVCircleThickness, 1, 8);
                ImGui::SliderFloat("Aimbot smoothness", &Globals::AimbotSmoothness, 1, 15);

                colorEdit4WithRainbow("FOV circle color", reinterpret_cast<float*>(&Globals::FOVCircleColor));

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc")) {
                ImGui::Checkbox("Team check", &Globals::TeamCheck);
                ImGui::Checkbox("Health check", &Globals::HealthCheck);
                ImGui::Checkbox("Crosshair", &Globals::Crosshair);

                ImGui::SliderFloat("Crosshair scale", &Globals::CrosshairScale, 0.1, 5);
                ImGui::SliderFloat("Crosshair thickness", &Globals::CrosshairThickness, 1, 6);
                ImGui::SliderFloat("Rainbow speed", &Globals::RainbowSpeed, 0.1, 8);

                colorEdit4WithRainbow("Crosshair color", reinterpret_cast<float*>(&Globals::CrosshairColor));

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Config")) {
                ImGui::InputText("Config name", configName, IM_ARRAYSIZE(configName));

                if (ImGui::Button("Save config")) {
                    Config::Save(configName);
                }
                if (ImGui::Button("Load config")) {
                    Config::Load(configName);
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
            ImGui::End();
        }

        ImGui::EndFrame();

        for (const auto& col : rainbowColors) {
            if (col.second.second) {
                Utility::HSVToRGB(rainbowIteration / MAX_RAINBOW_ITERATION, 1, 1, col.second.second);
            }
        }

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
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

    switch (msg) {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
                g_d3dpp.BackBufferWidth = LOWORD(lParam);
                g_d3dpp.BackBufferHeight = HIWORD(lParam);
                ResetDevice();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            exit(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}