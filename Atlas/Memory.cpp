#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"

constexpr int MAX_READ_LENGTH = 200;

std::uintptr_t Memory::GetProcessID(const wchar_t* processName) {
	std::uintptr_t processID = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		std::cout << "Couldn't get running processes.\n";
		return 0;
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot, &processEntry)) {
		CloseHandle(snapshot);
		std::cout << "Couldn't get info of processes.\n";
		return 0;
	}

	do {
		if (!_wcsicmp(processEntry.szExeFile, processName)) {
			processID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &processEntry));

	CloseHandle(snapshot);

	return processID;
}

std::uintptr_t Memory::GetModuleBaseAddress(std::uintptr_t processID, const wchar_t* moduleName) {
	std::uintptr_t moduleBaseAddress = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
	if (snapshot == INVALID_HANDLE_VALUE) {
		std::cout << "Couldn't get running processes.\n";
		return 0;
	}

	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(moduleEntry);

	if (!Module32First(snapshot, &moduleEntry)) {
		CloseHandle(snapshot);
		std::cout << "Couldn't get info of modules.\n";
		return 0;
	}

	do {
		if (!_wcsicmp(moduleEntry.szModule, moduleName)) {
			moduleBaseAddress = reinterpret_cast<std::uintptr_t>(moduleEntry.modBaseAddr);
			break;
		}
	} while (Module32Next(snapshot, &moduleEntry));

	CloseHandle(snapshot);

	return moduleBaseAddress;
}

std::uintptr_t Memory::Scan(std::uintptr_t VFTableAddress) {
	SYSTEM_INFO systemInfo;
	std::uintptr_t pageSize;
	std::uintptr_t pageSize4ByteSplit;
	MEMORY_BASIC_INFORMATION memoryInfo;
	GetSystemInfo(&systemInfo);
	pageSize = systemInfo.dwPageSize;
	pageSize4ByteSplit = pageSize / 4;
	std::uintptr_t* buffer = new std::uintptr_t[pageSize];

	for (std::uintptr_t addr = Globals::BaseAddress; addr < 0x7FFFFFFF; addr += pageSize) {
		VirtualQueryEx(Globals::Handle, reinterpret_cast<LPCVOID>(addr), &memoryInfo, pageSize);
		if (memoryInfo.Protect == PAGE_READWRITE) {
			ReadProcessMemory(Globals::Handle, reinterpret_cast<LPCVOID>(addr), buffer, pageSize, 0);
			for (std::uintptr_t i = 0; i <= pageSize4ByteSplit; i++) {
				if (buffer[i] == VFTableAddress) {
					delete[] buffer;
					return static_cast<std::uintptr_t>(addr + (i * 4));
				}
			}
		}
	}

	delete[] buffer;
	return 0;
}

std::string Memory::ReadStringOfUnknownLength(std::uintptr_t address) {
	std::string string;
	char character = 0;
	int charSize = sizeof(character);
	int offset = 0;

	string.reserve(52);

	while (offset < MAX_READ_LENGTH) { // 200 / sizeof(char) == 50. This means 50 charactes can be read until it will stop reading.
		character = Read<char>(reinterpret_cast<LPCVOID>(address + offset));
		if (character == 0) break;
		offset += charSize;
		string.push_back(character);
	}

	return string;
}