#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"
#include "Globals.h"

HANDLE handle;

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

void Memory::SetHandle(HANDLE handle_) {
	Globals::Handle = handle_;
	handle = handle_;
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
		VirtualQueryEx(handle, (LPCVOID)addr, &memoryInfo, pageSize);
		if (memoryInfo.Protect == PAGE_READWRITE) {
			ReadProcessMemory(handle, (LPCVOID)addr, buffer, pageSize, 0);
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

template<typename T>
T Memory::Read(LPCVOID address, SIZE_T size) {
	T buffer;
	ReadProcessMemory(handle, address, &buffer, size, 0);
	return buffer;
}

template<typename T>
bool Memory::Write(LPVOID address, LPCVOID buffer, SIZE_T size) {
	return WriteProcessMemory(handle, address, buffer, size, 0);
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

void* Memory::CreateCharPointerString(const char* string) {
	void* stringMemory = VirtualAllocEx(handle, 0, strlen(string), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!stringMemory) {
		std::cout << "Couldn't allocate memory for \"" << string << "\" char pointer." << std::endl;
		return 0;
	}

	if (!WriteProcessMemory(handle, stringMemory, string, strlen(string), 0)) {
		std::cout << "Couldn't write \"" << string << "\" to memory." << std::endl;
		return 0;
	}

	return stringMemory;
}

DWORD Memory::GetTeam(DWORD player) {
	return Memory::GetPointerAddress(player + 0x98);
}

DWORD Memory::GetCharacter(DWORD player) {
	return GetPointerAddress(player + 0x64);
}

std::string Memory::GetClassType(DWORD instance) {
	std::string className;

	DWORD classDescriptor = GetPointerAddress(instance + 0xC);
	className = ReadStringOfUnknownLength(GetPointerAddress(classDescriptor + 0x4));

	return className;
}

std::string Memory::GetName(DWORD instance) {
	uintptr_t nameAddress = GetPointerAddress(instance + 0x28);
	std::string name = ReadStringOfUnknownLength(nameAddress);

	int size = Read<int>((LPCVOID)(nameAddress + 0x14));

	if (size >= 16u) {
		uintptr_t newNameAddress = GetPointerAddress(nameAddress);
		return ReadStringOfUnknownLength(newNameAddress);
	} else {
		return name;
	}
}

std::vector<DWORD> Memory::GetChildren(DWORD instance) {
	std::vector<DWORD> children = {};

	DWORD v4 = GetPointerAddress(instance + 0x2C);
	int v25 = (GetPointerAddress(v4 + 4) - GetPointerAddress(v4)) >> 3;
	if (!v25) {
		std::cout << "Couldn't get number of children." << std::endl;
		return children;
	}

	DWORD v6 = GetPointerAddress(v4);
	for (int i = 0; i < v25; i++) {
		children.push_back(GetPointerAddress(v6));
		v6 += 8;
	}

	return children;
}

DWORD Memory::GetService(DWORD game, std::string className) {
	std::vector<DWORD> children = GetChildren(game);

	for (DWORD child : children) {
		if (GetClassType(child) == className) {
			return child;
		}
	}
}

std::vector<DWORD> Memory::GetPlayers(DWORD playersService) {
	std::vector<DWORD> players = {};

	std::vector<DWORD> children = GetChildren(playersService);

	for (DWORD child : children) {
		if (GetClassType(child) == "Player") {
			players.push_back(child);
		}
	}

	return players;
}

DWORD Memory::FindFirstChild(DWORD instance, std::string name) {
	std::vector<DWORD> children = GetChildren(instance);

	for (DWORD child : children) {
		if (GetName(child) == name) {
			return child;
		}
	}
}

Vector3 Memory::GetPosition(DWORD instance) {
	Vector3 position = { 0, 0, 0 };

	DWORD primitive = Read<DWORD>((LPCVOID)(instance + 0xA8));
	position = Read<Vector3>((LPCVOID)(primitive + 0x11C));

	return position;
}

Vector3 Memory::GetCameraPosition(DWORD visualEngine) {
	Vector3 position = { 0, 0, 0 };

	position = Read<Vector3>((LPCVOID)(visualEngine + 0x44));

	return position;
}