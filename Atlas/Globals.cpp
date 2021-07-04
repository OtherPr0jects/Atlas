#include <Windows.h>
#include "Globals.h"

HANDLE Globals::Handle = NULL;
DWORD Globals::BaseAddress = NULL;

HWND Globals::RobloxHWND = NULL;
HWND Globals::OverlayHWND = NULL;
HWND Globals::GuiHWND = NULL;

Players Globals::PlayersService = NULL;
Player Globals::LocalPlayer = NULL;
VisualEngine Globals::VisualEngine = NULL;

bool Globals::ESPBoxEnabled = true;
bool Globals::ESPNameEnabled = true;
bool Globals::ESPDistanceEnabled = true;

bool Globals::AimbotEnabled = true;
bool Globals::TeamCheck = false;
bool Globals::Crosshair = true;
bool Globals::ViewFOVCircle = true;

int Globals::Target = 2;
int Globals::AimbotBind = 0;

float Globals::CrosshairScale = 1;
float Globals::CrosshairThickness = 2;
float Globals::FOVSize = 100;
float Globals::AimbotSmoothness = 8;
float Globals::ESPBoxThickness = 2;

Color3 Globals::CrosshairColor = { 1, 0, 0 };
Color3 Globals::FOVCircleColor = { 1, 0, 0 };
Color3 Globals::ESPBoxColor = { 1, 0, 0 };
Color3 Globals::NameColor = { 1, 1, 1 };
Color3 Globals::DistanceColor = { 1, 1, 1 };