#include "RBXClasses.h"
#include "Memory.h"

float Humanoid::GetHealth() {
	return Memory::Read<float>(reinterpret_cast<LPCVOID>(Memory::GetPointerAddress(this->Address + 0x218)));
}