#pragma once
#include <Windows.h>

struct Addresses {
	// Contains offset of address from base address.

	// VFTables.
	static const std::uintptr_t DataModel = 0x1CF0080;
	static const std::uintptr_t VisualEngine = 0x1E24CA8;
};