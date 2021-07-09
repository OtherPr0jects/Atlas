#pragma once
#include "Globals.h"

class Memory {
public:
	static std::uintptr_t GetProcessID(const wchar_t* processName);

	static std::uintptr_t GetModuleBaseAddress(std::uintptr_t processID, const wchar_t* moduleName);

	static std::uintptr_t Scan(std::uintptr_t VFTableAddress);

	template<typename T>
	static inline T Read(LPCVOID address, SIZE_T size = sizeof(T)) { // Had to put the definition here because you can't define generic methods in the cpp file.
		T buffer;
		ReadProcessMemory(Globals::Handle, address, &buffer, size, 0);
		return buffer;
	};

	static std::uintptr_t GetPointerAddress(std::uintptr_t address);

	static std::uintptr_t GetDMAAddress(std::uintptr_t ptr, std::vector<std::uintptr_t> offsets);

	static std::string ReadStringOfUnknownLength(std::uintptr_t address);
};