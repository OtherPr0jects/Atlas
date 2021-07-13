#include <iostream>
#include <Windows.h>
#include <vector>
#include "Render.h"
#include "Memory.h"
#include "DirectOverlay.h"
#include "Globals.h"

constexpr Vector3 HEAD_OFFSET = { 0, 2, 0 };
constexpr Vector3 LEG_OFFSET = { 0, 3, 0 };

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

	Vector2 clientDimensions = Globals::VisualEngine.GetClientDimensions();
	Vector2 screenCenter = { clientDimensions.X / 2, clientDimensions.Y / 2 };

	if (Globals::ViewFOVCircle) {
		float* fovCircleColor = Globals::FOVCircleColor;

		DrawCircle(
			cursorPosition.x, cursorPosition.y,
			Globals::FOVSize - 10,
			Globals::FOVCircleThickness,
			fovCircleColor[0], fovCircleColor[1], fovCircleColor[2], fovCircleColor[3],
			false
		);
	}

	if (Globals::Crosshair) {
		float* crosshairColor = Globals::CrosshairColor;

		DrawLine(
			screenCenter.X - (20 * Globals::CrosshairScale), screenCenter.Y,
			screenCenter.X + (20 * Globals::CrosshairScale), screenCenter.Y,
			Globals::CrosshairThickness,
			crosshairColor[0], crosshairColor[1], crosshairColor[2], crosshairColor[3]
		);

		DrawLine(
			screenCenter.X, screenCenter.Y - (20 * Globals::CrosshairScale),
			screenCenter.X, screenCenter.Y + (20 * Globals::CrosshairScale),
			Globals::CrosshairThickness,
			crosshairColor[0], crosshairColor[1], crosshairColor[2], crosshairColor[3]
		);
	}

	if (!Globals::ESPBoxEnabled &&
		!Globals::ESPNameEnabled &&
		!Globals::ESPDistanceEnabled &&
		!Globals::ESPHeadDotEnabled &&
		!Globals::ESPTracerEnabled
	) return;

	Instance localTeam = Globals::TeamCheck ? Globals::LocalPlayer.GetTeam() : 0;

	/*std::uintptr_t localCharacter = Memory::GetCharacter(localPlayerR);
	if (!localCharacter) return;

	std::uintptr_t localHumanoidRootPart = Memory::FindFirstChild(localCharacter, "HumanoidRootPart");
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

		BasePart humanoidRootPart = static_cast<BasePart>(character.FindFirstChild("HumanoidRootPart").Address);
		if (!humanoidRootPart.Address) continue;

		Humanoid humanoid = static_cast<Humanoid>(character.FindFirstChildOfClass("Humanoid").Address);
		if (!humanoid.Address) continue;
		float health = humanoid.GetHealth();
		if (Globals::HealthCheck && health == 0) continue;

		Vector3 humanoidRootPartPosition = humanoidRootPart.GetPosition();

		//Vector2 humanoidRootPartScreenPos = Render::WorldToScreenPoint(humanoidRootPartPosition);
		//if (humanoidRootPartScreenPos.X == -1) continue;

		Vector2 headScreenPos = Render::WorldToScreenPoint(AddVector3(humanoidRootPartPosition, HEAD_OFFSET));
		if (headScreenPos.X == -1) continue;

		Vector2 legScreenPos = Render::WorldToScreenPoint(SubtractVector3(humanoidRootPartPosition, LEG_OFFSET));
		if (legScreenPos.X == -1) continue;

		//float distanceFromLocalHumanoidRootPartPosition = GetVector3Magnitude(SubtractVector3(humanoidRootPartPosition, localHumanoidRootPartPosition));
		float distanceFromCamera = GetVector3Magnitude(SubtractVector3(cameraPosition, humanoidRootPartPosition));

		float fontSize = 13;

		if (Globals::HealthBarEnabled) {
			float* espBoxColor = Globals::ESPBoxColor;

			float healthBarThickness = Globals::HealthBarThickness;

			float height = legScreenPos.Y - headScreenPos.Y;
			float width = (50 + healthBarThickness) / distanceFromCamera;

			float x = (headScreenPos.X - (height / 3.2));

			float outlineThickness = 70 / distanceFromCamera;

			float maxHealth = humanoid.GetMaxHealth();

			DrawBox( // Black outline.
				x - (outlineThickness / 2), headScreenPos.Y - (outlineThickness / 2),
				width + outlineThickness, height + outlineThickness,
				0,
				0, 0, 0, espBoxColor[3],
				true
			);

			DrawBox(
				x, legScreenPos.Y,
				width, -(height * (health / maxHealth)),
				0,
				0, 0.85, 0, espBoxColor[3],
				true
			);
		}
		if (Globals::ESPBoxEnabled) {
			float* espBoxColor = Globals::ESPBoxColor;

			float height = legScreenPos.Y - headScreenPos.Y;
			float width = height / 2;

			float espBoxThickness = Globals::ESPBoxThickness;

			DrawBox( // Black outline.
				headScreenPos.X - (width / 2), headScreenPos.Y,
				width, height,
				espBoxThickness + 2,
				0, 0, 0, espBoxColor[3],
				false
			);

			DrawBox(
				headScreenPos.X - (width / 2), headScreenPos.Y,
				width, height,
				espBoxThickness,
				espBoxColor[0], espBoxColor[1], espBoxColor[2], espBoxColor[3],
				false
			);
		}
		if (Globals::ESPTracerEnabled) {
			bool fromBottomCenter = (Globals::TracerStartLocation == 0);

			float* tracerColor = Globals::TracerColor;

			DrawLine(
				fromBottomCenter ? screenCenter.X : cursorPosition.x, fromBottomCenter ? clientDimensions.Y : cursorPosition.y,
				legScreenPos.X, legScreenPos.Y,
				Globals::TracerThickness,
				tracerColor[0], tracerColor[1], tracerColor[2], tracerColor[3]
			);
		}
		if (Globals::ESPNameEnabled) {
			float *nameColor = Globals::NameColor;

			DrawString(
				player.GetName(),
				fontSize,
				headScreenPos.X, headScreenPos.Y - 25,
				nameColor[0], nameColor[1], nameColor[2], nameColor[3]
			);
		}
		if (Globals::ESPDistanceEnabled) {
			float* distanceColor = Globals::DistanceColor;

			DrawString(
				"[" + std::to_string(static_cast<int>(distanceFromCamera)) + "s]",
				fontSize,
				headScreenPos.X, legScreenPos.Y + 5,
				distanceColor[0], distanceColor[1], distanceColor[2], distanceColor[3]
			);
		}
		if (Globals::ESPHeadDotEnabled) {
			BasePart head = static_cast<BasePart>(character.FindFirstChild("Head").Address);
			if (!head.Address) continue;

			Vector3 headPosition = head.GetPosition();

			Vector2 realHeadScreenPos = Render::WorldToScreenPoint(headPosition);
			if (realHeadScreenPos.X == -1) continue;

			float* headDotColor = Globals::HeadDotColor;

			DrawCircle(
				realHeadScreenPos.X, realHeadScreenPos.Y,
				500 / distanceFromCamera,
				0,
				headDotColor[0], headDotColor[1], headDotColor[2], headDotColor[3],
				true
			);
		}
	}
}

Vector2 Render::WorldToScreenPoint(Vector3 position) {
	Vector2 clientDimensions = Globals::VisualEngine.GetClientDimensions();

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
		(clientDimensions.X / 2 * NDC.X) + (NDC.X + clientDimensions.X / 2),
		-(clientDimensions.Y / 2 * NDC.Y) + (NDC.Y + clientDimensions.Y / 2)
	};
}

void Render::Setup() {
	DirectOverlaySetOption(D2DOV_FONT_COURIER | D2DOV_REQUIRE_FOREGROUND);
	DirectOverlaySetup(drawLoop, Globals::RobloxHWND);
}