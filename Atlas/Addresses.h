#pragma once
#include <Windows.h>

struct Addresses {
	// Contains offset of address from base address.

	// VFTables.
	static const std::uintptr_t ScriptContext = 0x1D737A8; // Using ScriptContext to get the DataModel object because sometimes scanning for the DataModel VFTable leads to multiple results.
	static const std::uintptr_t VisualEngine = 0x1F43B98;
};