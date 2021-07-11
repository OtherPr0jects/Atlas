#include "RBXClasses.h"
#include "Memory.h"

std::int64_t Player::GetUserID() {
	return Memory::Read<__int64>(reinterpret_cast<LPCVOID>(this->Address + 0xF8));
}

Instance Player::GetTeam() {
	return static_cast<Instance>(Memory::GetPointerAddress(this->Address + 0xB0));
}

Instance Player::GetCharacter() {
	return static_cast<Instance>(Memory::GetPointerAddress(this->Address + 0x7C));
}