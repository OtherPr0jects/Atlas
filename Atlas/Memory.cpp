#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"

DWORD Memory::GetProcessID(const wchar_t* processName) {
	DWORD processID = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		std::cout << "Couldn't get running processes." << std::endl;
		return 0;
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot, &processEntry)) {
		CloseHandle(snapshot);
		std::cout << "Couldn't get info of processes." << std::endl;
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

uintptr_t Memory::GetModuleBaseAddress(DWORD processID, const wchar_t* moduleName) {
	uintptr_t moduleBaseAddress = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
	if (snapshot == INVALID_HANDLE_VALUE) {
		std::cout << "Couldn't get running processes." << std::endl;
		return 0;
	}

	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(moduleEntry);

	if (!Module32First(snapshot, &moduleEntry)) {
		CloseHandle(snapshot);
		std::cout << "Couldn't get info of modules." << std::endl;
		return 0;
	}

	do {
		if (!_wcsicmp(moduleEntry.szModule, moduleName)) {
			moduleBaseAddress = (uintptr_t)moduleEntry.modBaseAddr;
			break;
		}
	} while (Module32Next(snapshot, &moduleEntry));

	CloseHandle(snapshot);

	return moduleBaseAddress;
}

DWORD Memory::Scan(DWORD baseAddress, DWORD VFTableAddress) {
	SYSTEM_INFO systemInfo;
	DWORD pageSize;
	DWORD pageSize4ByteSplit;
	MEMORY_BASIC_INFORMATION memoryInfo;
	GetSystemInfo(&systemInfo);
	pageSize = systemInfo.dwPageSize;
	pageSize4ByteSplit = pageSize / 4;
	DWORD* buffer = new DWORD[pageSize];

	for (DWORD addr = baseAddress; addr < 0x7FFFFFFF; addr += pageSize) {
		VirtualQueryEx(Globals::Handle, (LPCVOID)addr, &memoryInfo, pageSize);
		if (memoryInfo.Protect == PAGE_READWRITE) {
			ReadProcessMemory(Globals::Handle, (LPCVOID)addr, buffer, pageSize, 0);
			for (DWORD i = 0; i <= pageSize4ByteSplit; i++) {
				if (buffer[i] == VFTableAddress) {
					delete[] buffer;
					return (DWORD)(addr + (i * 4));
				}
			}
		}
	}

	delete[] buffer;
	return 0;
}

DWORD Memory::GetPointerAddress(DWORD address) {
	uintptr_t pointerAddress = GetDMAAddress(address, { 0x0 });
	return pointerAddress;
}

uintptr_t Memory::GetDMAAddress(uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		addr = Read<uintptr_t>((LPCVOID)addr);
		//ReadProcessMemory(handle, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

std::string Memory::ReadStringOfUnknownLength(DWORD address) {
	std::string string;
	char character = 0;
	int charSize = sizeof(character);
	int offset = 0;

	while (true) {
		character = Read<char>((LPCVOID)(address + offset));
		if (character == 0) break;
		offset += charSize;
		string.push_back(character);
	}

	return string;
}