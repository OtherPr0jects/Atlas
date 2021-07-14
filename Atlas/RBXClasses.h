#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "RBXStructs.h"

class Instance {
	public:
		Instance(std::uintptr_t address) : Address(address) {};

		std::uintptr_t Address;

		Instance GetParent();

		std::string GetName();

		std::string GetClassType(); // GetClassName.

		std::vector<Instance> GetChildren();

		Instance FindFirstChild(std::string name);

		Instance FindFirstChildOfClass(std::string className);
};

class VisualEngine {
	public:
		VisualEngine(std::uintptr_t address) : Address(address) {};

		std::uintptr_t Address;

		Vector2 GetClientDimensions();

		float* GetViewMatrix();

		Vector3 GetCameraPosition();
};

class DataModel : public Instance {
	public:
		using Instance::Instance;

		Instance GetService(std::string className);
};

class Player : public Instance {
	public:
		using Instance::Instance;

		Instance GetTeam();

		Instance GetCharacter();
};

class Players : public Instance {
	public:
		using Instance::Instance;

		Player GetLocalPlayer();

		std::vector<Player> GetPlayers();
};

class BasePart : public Instance {
	public:
		using Instance::Instance;

		Vector3 GetPosition();
};

class Humanoid : public Instance {
	public:
		using Instance::Instance;

		float GetHealth();

		float GetMaxHealth();
};