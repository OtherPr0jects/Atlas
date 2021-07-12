#include "RBXClasses.h"
#include "Memory.h"

Vector2 VisualEngine::GetClientDimensions() {
	Vector2 clientDimensions = { 0, 0 };

	std::uintptr_t offset = 0x4D8;
	return {
		static_cast<float>(Memory::Read<int>(reinterpret_cast<LPCVOID>(this->Address + offset))), // Width.
		static_cast<float>(Memory::Read<int>(reinterpret_cast<LPCVOID>(this->Address + offset + sizeof(int)))) // Height.
	};
}

float* VisualEngine::GetViewMatrix() {
	float viewMatrix[16];

	ReadProcessMemory(Globals::Handle, reinterpret_cast<LPCVOID>(this->Address + 0x120), &viewMatrix, sizeof(viewMatrix), 0);

	return viewMatrix;
}

Vector3 VisualEngine::GetCameraPosition() {
	return Memory::Read<Vector3>(reinterpret_cast<LPCVOID>(this->Address + 0x44));
}