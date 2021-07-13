#pragma once
#include "Globals.h"

class Memory {
public:
	static std::uintptr_t GetProcessID(const wchar_t* processName);

	static std::uintptr_t GetModuleBaseAddress(std::uintptr_t processID, const wchar_t* moduleName);

	static std::uintptr_t Scan(std::uintptr_t VFTableAddress);

	template<typename T>
	static inline T Read(LPCVOID address, std::size_t size = sizeof(T)) { // Had to put the definition here because you can't define generic methods in the cpp file.
		T buffer;
		ReadProcessMemory(Globals::Handle, address, &buffer, size, 0);
		return buffer;
	};

	static inline std::uintptr_t GetPointerAddress(std::uintptr_t address) { // Had to put the definition here because you can't define inline methods in the cpp file.
		return Read<std::uintptr_t>(reinterpret_cast<LPCVOID>(address));
	}

	static std::string ReadStringOfUnknownLength(std::uintptr_t address);
};