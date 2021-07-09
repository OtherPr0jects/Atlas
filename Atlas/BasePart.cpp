#include "RBXClasses.h"
#include "Memory.h"

Vector3 BasePart::GetPosition() {
	std::uintptr_t primitive = Memory::Read<std::uintptr_t>(reinterpret_cast<LPCVOID>(this->Address + 0xC0));
	return Memory::Read<Vector3>(reinterpret_cast<LPCVOID>(primitive + 0x11C));
}