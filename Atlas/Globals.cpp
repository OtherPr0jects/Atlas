#include <Windows.h>
#include "Globals.h"

HANDLE Globals::Handle = NULL;
HWND Globals::RobloxHWND = NULL;
HWND Globals::OverlayHWND = NULL;
HWND Globals::GuiHWND = NULL;
DWORD Globals::PlayersService = NULL;
DWORD Globals::LocalPlayer = NULL;
DWORD Globals::VisualEngine = NULL;
bool Globals::ESPEnabled = true;
bool Globals::AimbotEnabled = true;
bool Globals::TeamCheck = false;
bool Globals::Crosshair = true;
bool Globals::ViewFOVCircle = true;
int Globals::Target = 3;
int Globals::AimbotBind = 0;
float Globals::CrosshairScale = 1;
float Globals::CrosshairThickness = 2;
float Globals::FOVSize = 100;
float Globals::AimbotSmoothness = 8;
Color3 Globals::CrosshairColor = { 1, 0, 0 };
Color3 Globals::FOVCircleColor = { 1, 0, 0 };
Color3 Globals::ESPBoxColor = { 1, 0, 0 };
Color3 Globals::NameColor = { 1, 1, 1 };
Color3 Globals::DistanceColor = { 1, 1, 1 };