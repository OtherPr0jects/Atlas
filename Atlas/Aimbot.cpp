#include <iostream>
#include <Windows.h>
#include <vector>
#include "Aimbot.h"
#include "Render.h"
#include "Memory.h"
#include "Globals.h"
#include "RBXStructs.h"

int aimbotBindKeycodes[] = {
    VK_RBUTTON,
    VK_LBUTTON,
    VK_LSHIFT
};

Vector2 SubtractVector2(Vector2 operand1, Vector2 operand2) {
    return {
        operand1.X - operand2.X,
        operand1.Y - operand2.Y
    };
}

float GetVector2Magnitude(Vector2 vector) {
    return sqrtf((vector.X * vector.X) + (vector.Y * vector.Y));
}

Vector2 getClosestCharacterScreenPosToCursor() {
    //DWORD closestCharacter = 0;
    Vector2 screenPos = { -1, -1 };
    int shortestDistance = 9e9;

    DWORD localTeam = 0;
    if (Globals::TeamCheck) {
        localTeam = Memory::GetTeam(Globals::LocalPlayer);
    }

	std::vector<DWORD> players = Memory::GetPlayers(Globals::PlayersService);
	for (DWORD player : players) {
        if (player == Globals::LocalPlayer) continue;

        DWORD team = Memory::GetTeam(player);
        if (Globals::TeamCheck && (team == localTeam)) continue;

		DWORD character = Memory::GetCharacter(player);
		if (!character) continue;

        //DWORD humanoid = Memory::FindFirstChild(handle, character, "Humanoid");
        //if (!humanoid || Memory::GetHealth(handle, humanoid) == 0) continue;

        DWORD targetInstance;
        
        if (Globals::Target == 0) {
            targetInstance = Memory::FindFirstChild(character, "Head");
            if (!targetInstance) continue;
        } else if (Globals::Target == 1) {
            targetInstance = Memory::FindFirstChild(character, "HumanoidRootPart");
        } else {
            targetInstance = ((rand() % 100 + 1) < 50) ? Memory::FindFirstChild(character, "HumanoidRootPart") :
                Memory::FindFirstChild(character, "Head");
        }
        if (!targetInstance) continue;
        
        Vector2 targetScreenPos = Render::WorldToScreenPoint(Memory::GetPosition(targetInstance));
        if (targetScreenPos.X == -1) continue;

        Vector2 cursorPos;
        POINT temp;
        GetCursorPos(&temp);
        cursorPos = { (float)temp.x, (float)temp.y };

        float magnitude = GetVector2Magnitude(SubtractVector2(targetScreenPos, cursorPos));
        
        if ((magnitude < Globals::FOVSize) && (magnitude < shortestDistance)) {
            //closestCharacter = character;
            screenPos = targetScreenPos;
            shortestDistance = magnitude;
        }
	}

    return screenPos;
}

Vector2 getRelative(Vector2 position) {
    Vector2 newPosition = { 0, 0 };

    Vector2 cursorPosition;
    POINT temp;
    GetCursorPos(&temp);
    ScreenToClient(Globals::RobloxHWND, &temp);
    cursorPosition = { (float)temp.x, (float)temp.y };

    newPosition.X = (position.X - cursorPosition.X) / Globals::AimbotSmoothness;
    newPosition.Y = (position.Y - cursorPosition.Y) / Globals::AimbotSmoothness;

    return newPosition;
}

void Aimbot::Setup() {
	while (true) {
        if (!Globals::AimbotEnabled) continue;

        if (GetForegroundWindow() != Globals::RobloxHWND) continue;

        if (GetAsyncKeyState(aimbotBindKeycodes[Globals::AimbotBind])) {
		    Vector2 closestCharacterScreenPos = getClosestCharacterScreenPosToCursor();
            if (closestCharacterScreenPos.X == -1) continue;

            Vector2 relative = getRelative(closestCharacterScreenPos);

            INPUT input;
            //ZeroMemory(&input, sizeof(INPUT));
            input.mi.time = 0; // Alternative to ZeroMemory (stops screensaver from mode)
            input.type = INPUT_MOUSE;
            input.mi.mouseData = 0;
            input.mi.dx = relative.X;
            input.mi.dy = relative.Y;
            input.mi.dwFlags = MOUSEEVENTF_MOVE;
            SendInput(1, &input, sizeof(input));
        }

        Sleep(1);
	}
}