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
    //std::uintptr_t closestCharacter = 0;
    Vector2 screenPos = { -1, -1 };
    int shortestDistance = 9e9;

    Instance localTeam = Globals::TeamCheck ? Globals::LocalPlayer.GetTeam() : 0;

	std::vector<Player> players = Globals::PlayersService.GetPlayers();
	for (Player player : players) {
        if (player.Address == Globals::LocalPlayer.Address) continue;

        Instance team = player.GetTeam();
        if (Globals::TeamCheck && (team.Address == localTeam.Address)) continue;

        Instance character = player.GetCharacter();
		if (!character.Address) continue;

        Humanoid humanoid = static_cast<Humanoid>(character.FindFirstChildOfClass("Humanoid").Address);
        if (!humanoid.Address) continue;
        if (humanoid.GetHealth() == 0) continue;

        BasePart targetInstance(0);
        
        if (Globals::Target == 0) {
            targetInstance = static_cast<BasePart>(character.FindFirstChild("Head").Address);
        } else if (Globals::Target == 1) {
            targetInstance = static_cast<BasePart>(character.FindFirstChild("HumanoidRootPart").Address);
        } else {
            targetInstance = ((rand() % 100 + 1) < 50) ? static_cast<BasePart>(character.FindFirstChild("HumanoidRootPart").Address) :
                static_cast<BasePart>(character.FindFirstChild("Head").Address);
        }
        if (!targetInstance.Address) continue;
        
        Vector2 targetScreenPos = Render::WorldToScreenPoint(targetInstance.GetPosition());
        if (targetScreenPos.X == -1) continue;

        POINT cursorPosition;
        GetCursorPos(&cursorPosition);
        ScreenToClient(Globals::RobloxHWND, &cursorPosition);

        float magnitude = GetVector2Magnitude(SubtractVector2(targetScreenPos, {
            static_cast<float>(cursorPosition.x),
            static_cast<float>(cursorPosition.y)
        }));
        
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

    POINT cursorPosition;
    GetCursorPos(&cursorPosition);
    ScreenToClient(Globals::RobloxHWND, &cursorPosition);

    newPosition.X = (position.X - cursorPosition.x) / Globals::AimbotSmoothness;
    newPosition.Y = (position.Y - cursorPosition.y) / Globals::AimbotSmoothness;

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
            input.mi.time = 0; // Alternative to ZeroMemory (stops screensaver from activating)
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