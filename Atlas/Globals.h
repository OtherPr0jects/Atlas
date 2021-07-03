#pragma once
#include <Windows.h>
#include "RBXStructs.h"

struct Globals {
	static HANDLE Handle;
	static HWND RobloxHWND;
	static HWND OverlayHWND;
	static HWND GuiHWND;
	static DWORD PlayersService;
	static DWORD LocalPlayer;
	static DWORD VisualEngine;
	static bool ESPEnabled;
	static bool AimbotEnabled;
	static bool TeamCheck;
	static bool Crosshair;
	static int Target;
	static int AimbotBind;
	static float CrosshairScale;
	static float CrosshairThickness;
	static float FOVSize;
	static float AimbotSmoothness;
	static Color3 CrosshairColor;
	static Color3 ESPBoxColor;
	static Color3 NameColor;
	static Color3 DistanceColor;
};