#include <iostream>
#include "RBXClasses.h"
#include "Memory.h"

std::string Instance::GetClassType() {
	std::uintptr_t classDescriptor = Memory::GetPointerAddress(this->Address + 0xC);
	return Memory::ReadStringOfUnknownLength(Memory::GetPointerAddress(classDescriptor + 0x4));
}

std::string Instance::GetName() {
	std::uintptr_t nameAddress = Memory::GetPointerAddress(this->Address + 0x28);
	std::string name = Memory::ReadStringOfUnknownLength(nameAddress);

	int size = Memory::Read<int>(reinterpret_cast<LPCVOID>(nameAddress + 0x14));

	if (size >= 16u) {
		std::uintptr_t newNameAddress = Memory::GetPointerAddress(nameAddress);
		return Memory::ReadStringOfUnknownLength(newNameAddress);
	} else {
		return name;
	}
}

std::vector<Instance> Instance::GetChildren() {
	std::vector<Instance> children = {};

	std::uintptr_t v4 = Memory::GetPointerAddress(this->Address + 0x2C);
	int v25 = (Memory::GetPointerAddress(v4 + 4) - Memory::GetPointerAddress(v4)) >> 3;
	if (!v25) {
		std::cout << "Couldn't get number of children of " << this->GetName() << ".\n";
		return children;
	}
	children.reserve(v25);

	std::uintptr_t v6 = Memory::GetPointerAddress(v4);
	for (int i = 0; i < v25; i++) {
		children.emplace_back(Memory::GetPointerAddress(v6));
		v6 += 8;
	}

	return children;
}

Instance Instance::FindFirstChild(std::string name) {
	std::vector<Instance> children = GetChildren();

	for (Instance child : children) {
		if (child.GetName() == name) {
			return static_cast<Instance>(child);
		}
	}

	return Instance(0);
}

Instance Instance::FindFirstChildOfClass(std::string className) {
	std::vector<Instance> children = GetChildren();

	for (Instance child : children) {
		if (child.GetClassType() == className) {
			return static_cast<Instance>(child);
		}
	}

	return Instance(0);
}