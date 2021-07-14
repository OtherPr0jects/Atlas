#include "RBXClasses.h"
#include "Memory.h"

Instance Player::GetTeam() {
	return static_cast<Instance>(Memory::GetPointerAddress(this->Address + 0xB0));
}

Instance Player::GetCharacter() {
	return static_cast<Instance>(Memory::GetPointerAddress(this->Address + 0x7C));
}