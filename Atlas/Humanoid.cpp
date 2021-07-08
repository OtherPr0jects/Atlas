#include "RBXClasses.h"
#include "Memory.h"

#include <iostream>

float Humanoid::GetHealth() {
	return Memory::Read<float>((LPCVOID)Memory::GetPointerAddress(this->Address + 0x218));
}