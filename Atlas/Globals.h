#pragma once
#include <Windows.h>
#include "RBXClasses.h"
#include "RBXStructs.h"

struct Globals {
	static HANDLE Handle;
	static DWORD BaseAddress;
	
	static HWND RobloxHWND;
	static HWND OverlayHWND;
	static HWND GuiHWND;

	static Players PlayersService;
	static Player LocalPlayer;
	static VisualEngine VisualEngine;

	static bool ESPBoxEnabled;
	static bool ESPNameEnabled;
	static bool ESPDistanceEnabled;

	static bool AimbotEnabled;
	static bool TeamCheck;
	static bool Crosshair;
	static bool ViewFOVCircle;

	static int Target;
	static int AimbotBind;

	static float CrosshairScale;
	static float CrosshairThickness;
	static float FOVSize;
	static float AimbotSmoothness;
	static float ESPBoxThickness;

	static Color3 CrosshairColor;
	static Color3 FOVCircleColor;
	static Color3 ESPBoxColor;
	static Color3 NameColor;
	static Color3 DistanceColor;
};