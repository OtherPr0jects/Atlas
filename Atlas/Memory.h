#pragma once
#include <vector>
#include "RBXStructs.h"

class Memory {
	public:
		HWND RobloxHWND;
		static DWORD GetProcessID(const wchar_t* processName);

		static uintptr_t GetModuleBaseAddress(DWORD processID, const wchar_t* moduleName);

		static void SetHandle(HANDLE handle_);

		static DWORD Scan(DWORD baseAddress, DWORD VFTableAddress);

		template <typename T>
		static T Read(LPCVOID address, SIZE_T size = sizeof(T));

		template <typename T>
		static bool Write(LPVOID address, LPCVOID buffer, SIZE_T size = sizeof(T));

		static DWORD GetPointerAddress(DWORD address);

		static uintptr_t GetDMAAddress(uintptr_t ptr, std::vector<unsigned int> offsets);

		static std::string ReadStringOfUnknownLength(DWORD address);

		static void* CreateCharPointerString(const char* string);

		static DWORD GetTeam(DWORD player);

		static DWORD GetCharacter(DWORD player);

		static std::string GetName(DWORD instance);

		static std::string GetClassType(DWORD instance); // GetClassName.

		static std::vector<DWORD> GetChildren(DWORD instance);

		static std::vector<DWORD> GetPlayers(DWORD playersService);

		static DWORD GetService(DWORD game, std::string className);

		static DWORD FindFirstChild(DWORD instance, std::string name);

		static Vector3 GetPosition(DWORD instance);

		static Vector3 GetCameraPosition(DWORD visualEngine);
};