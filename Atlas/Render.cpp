#include <iostream>
#include <Windows.h>
#include <vector>
#include "Render.h"
#include "Memory.h"
#include "DirectOverlay.h"
#include "Globals.h"

Vector3 AddVector3(Vector3 operand1, Vector3 operand2) {
	return {
		operand1.X + operand2.X,
		operand1.Y + operand2.Y,
		operand1.Z + operand2.Z
	};
}

Vector3 SubtractVector3(Vector3 operand1, Vector3 operand2) {
	return {
		operand1.X - operand2.X,
		operand1.Y - operand2.Y,
		operand1.Z - operand2.Z
	};
}

float GetVector3Magnitude(Vector3 vector) {
	return sqrtf((vector.X * vector.X) + (vector.Y * vector.Y) + (vector.Z * vector.Z));
}

void drawLoop(int width, int height) {
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);
	ScreenToClient(Globals::RobloxHWND, &cursorPosition);

	if (Globals::ViewFOVCircle) {
		Color3 fovCircleColor = Globals::FOVCircleColor;

		DrawCircle(
			cursorPosition.x, cursorPosition.y,
			Globals::FOVSize - 10,
			3,
			fovCircleColor.R, fovCircleColor.G, fovCircleColor.B, 1,
			false
		);
	}

	if (Globals::Crosshair) {
		Vector2 windowDimensions = Globals::VisualEngine.GetClientDimensions();
		Vector2 screenCenter = { windowDimensions.X / 2, windowDimensions.Y / 2 };

		Color3 crosshairColor = Globals::CrosshairColor;

		DrawLine(
			screenCenter.X - (20 * Globals::CrosshairScale), screenCenter.Y,
			screenCenter.X + (20 * Globals::CrosshairScale), screenCenter.Y,
			Globals::CrosshairThickness,
			crosshairColor.R, crosshairColor.G, crosshairColor.B, 1
		);

		DrawLine(
			screenCenter.X, screenCenter.Y - (20 * Globals::CrosshairScale),
			screenCenter.X, screenCenter.Y + (20 * Globals::CrosshairScale),
			Globals::CrosshairThickness,
			crosshairColor.R, crosshairColor.G, crosshairColor.B, 1
		);
	}

	if (!Globals::ESPBoxEnabled && ! Globals::ESPNameEnabled && !Globals::ESPDistanceEnabled) return;

	Color3 espBoxColor = Globals::ESPBoxColor;
	Color3 nameColor = Globals::NameColor;
	Color3 distanceColor = Globals::DistanceColor;

	Instance localTeam = 0;
	if (Globals::TeamCheck) {
		localTeam = Globals::LocalPlayer.GetTeam();
	}

	/*DWORD localCharacter = Memory::GetCharacter(localPlayerR);
	if (!localCharacter) return;

	DWORD localHumanoidRootPart = Memory::FindFirstChild(localCharacter, "HumanoidRootPart");
	if (!localHumanoidRootPart) return;

	Vector3 localHumanoidRootPartPosition = Memory::GetPosition(localHumanoidRootPart);
	if (localHumanoidRootPartPosition.X == -1) return;*/

	Vector3 cameraPosition = Globals::VisualEngine.GetCameraPosition();

	std::vector<Player> players = Globals::PlayersService.GetPlayers();
	for (Player player : players) {
		if (player.Address == Globals::LocalPlayer.Address) continue;

		Instance team = player.GetTeam();
		if (Globals::TeamCheck && (team.Address == localTeam.Address)) continue;

		Instance character = player.GetCharacter();
		if (!character.Address) continue;

		BasePart humanoidRootPart = BasePart(character.FindFirstChild("HumanoidRootPart").Address);
		//DWORD humanoid = Memory::FindFirstChild(handle, character, "Humanoid");

		if (!humanoidRootPart.Address/* || !humanoid*/) continue;
		//if (Memory::GetHealth(handle, humanoid) == 0) continue;

		Vector3 humanoidRootPartPosition = humanoidRootPart.GetPosition();

		//Vector2 humanoidRootPartScreenPos = Render::WorldToScreenPoint(humanoidRootPartPosition);
		//if (humanoidRootPartScreenPos.X == -1) continue;

		Vector2 headScreenPos = Render::WorldToScreenPoint(AddVector3(humanoidRootPartPosition, { 0, 2, 0 }));
		if (headScreenPos.X == -1) continue;

		Vector2 legScreenPos = Render::WorldToScreenPoint(SubtractVector3(humanoidRootPartPosition, { 0, 3, 0 }));
		if (legScreenPos.X == -1) continue;

		//float distanceFromLocalHumanoidRootPartPosition = GetVector3Magnitude(SubtractVector3(humanoidRootPartPosition, localHumanoidRootPartPosition));
		float distanceFromCamera = GetVector3Magnitude(SubtractVector3(cameraPosition, humanoidRootPartPosition));

		float fontSize = 13;

		if (Globals::ESPBoxEnabled) {
			float height = legScreenPos.Y - headScreenPos.Y;
			float width = height / 2;

			float espBoxThickness = Globals::ESPBoxThickness;

			DrawBox(
				headScreenPos.X - (width / 2), headScreenPos.Y,
				width, height,
				espBoxThickness + 2,
				0, 0, 0, 1,
				false
			);

			DrawBox(
				headScreenPos.X - (width / 2), headScreenPos.Y,
				width, height,
				espBoxThickness,
				espBoxColor.R, espBoxColor.G, espBoxColor.B, 1,
				false
			);
		}
		if (Globals::ESPNameEnabled) {
			DrawString(
				player.GetName(),
				fontSize,
				headScreenPos.X, headScreenPos.Y - 25,
				nameColor.R, nameColor.G, nameColor.B, 1
			);
		}
		if (Globals::ESPDistanceEnabled) {
			DrawString(
				"[" + std::to_string((int)distanceFromCamera) + "s]",
				fontSize,
				headScreenPos.X, legScreenPos.Y + 5,
				distanceColor.R, distanceColor.G, distanceColor.B, 1
			);
		}
	}
}

Vector2 Render::WorldToScreenPoint(Vector3 position) {
	Vector2 windowDimensions = Globals::VisualEngine.GetClientDimensions();

	float* viewMatrix;
	viewMatrix = Globals::VisualEngine.GetViewMatrix();
	
	Vector4 clipCoords;
	clipCoords.X = (position.X * viewMatrix[0]) + (position.Y * viewMatrix[1]) + (position.Z * viewMatrix[2]) + viewMatrix[3];
	clipCoords.Y = (position.X * viewMatrix[4]) + (position.Y * viewMatrix[5]) + (position.Z * viewMatrix[6]) + viewMatrix[7];
	clipCoords.Z = (position.X * viewMatrix[8]) + (position.Y * viewMatrix[9]) + (position.Z * viewMatrix[10]) + viewMatrix[11];
	clipCoords.W = (position.X * viewMatrix[12]) + (position.Y * viewMatrix[13]) + (position.Z * viewMatrix[14]) + viewMatrix[15];

	if (clipCoords.W < 0.1f) {
		return { -1, -1 }; // Off screen.
	}

	Vector3 NDC;
	NDC.X = clipCoords.X / clipCoords.W;
	NDC.Y = clipCoords.Y / clipCoords.W;
	NDC.Z = clipCoords.Z / clipCoords.W;

	return {
		(windowDimensions.X / 2 * NDC.X) + (NDC.X + windowDimensions.X / 2),
		-(windowDimensions.Y / 2 * NDC.Y) + (NDC.Y + windowDimensions.Y / 2)
	};
}

void Render::Setup() {
	DirectOverlaySetOption(D2DOV_FONT_COURIER | D2DOV_REQUIRE_FOREGROUND);
	DirectOverlaySetup(drawLoop, Globals::RobloxHWND);
}