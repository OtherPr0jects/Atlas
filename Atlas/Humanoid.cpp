#include "RBXClasses.h"
#include "Memory.h"
#include "Utility.h"

float Humanoid::GetHealth() { // Health in Roblox is obfuscated a little.
	std::uintptr_t offset = 0x218;
	std::uintptr_t v1 = Memory::GetPointerAddress(this->Address + offset);
	std::uintptr_t v2 = Memory::GetPointerAddress(Memory::GetPointerAddress(this->Address + offset));
	return Utility::HexToFloat(v2 - v1);
}

float Humanoid::GetMaxHealth() {
	std::uintptr_t offset = 0x21C;
	std::uintptr_t v1 = Memory::GetPointerAddress(this->Address + offset);
	std::uintptr_t v2 = Memory::GetPointerAddress(Memory::GetPointerAddress(this->Address + offset));
	return Utility::HexToFloat(v2 - v1);
}