#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"
#include "Addresses.h"
#include "Utility.h"
#include "Gui.h"
#include "Globals.h"

int main() {
    SetConsoleTitle((LPCWSTR)Utility::CreateRandomString(6).c_str());

    DWORD processID = Memory::GetProcessID(L"RobloxPlayerBeta.exe");
    if (!processID) {
        std::cout << "Couldn't get Roblox process ID." << std::endl;
        return 0;
    }

    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!handle) {
        std::cout << "Couldn't get Roblox handle." << std::endl;
        return 0;
    }
    Memory::SetHandle(handle);

    HWND hwnd = FindWindowA(0, "Roblox");
    Globals::RobloxHWND = hwnd;

    long long userID = 0;
    char username[21];
    std::string characterAppearanceID;
    std::string connectionLocation;
    std::string teamName;

    uintptr_t baseAddress = Memory::GetModuleBaseAddress(processID, L"RobloxPlayerBeta.exe");

    std::cout << "Welcome to Atlas. Scanning..." << std::endl;

    DWORD game = Memory::Scan(baseAddress, baseAddress + Addresses::DataModel);
    DWORD players = Memory::GetService(game, "Players");
    DWORD localPlayer = Memory::GetPointerAddress(players + 0x110);
    DWORD character = Memory::GetCharacter(localPlayer);
    DWORD team = Memory::GetTeam(localPlayer);

    DWORD visualEngine = Memory::Scan(baseAddress, baseAddress + Addresses::VisualEngine);

    Globals::PlayersService = players;
    Globals::LocalPlayer = localPlayer;
    Globals::VisualEngine = visualEngine;

    ReadProcessMemory(handle, (LPCVOID)(localPlayer + 0xF8), &userID, sizeof(userID), 0);

    ReadProcessMemory(handle, (LPCVOID)Memory::GetPointerAddress(localPlayer + 0x28), &username, sizeof(username), 0);

    if (team) {
        teamName = Memory::GetName(team);
    } else {
        teamName = "None";
    }

    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Base address: " << std::hex << baseAddress << std::dec << std::endl;
    std::cout << "DataModel address: " << std::hex << game << std::dec << std::endl;
    std::cout << "Players address: " << std::hex << players << std::dec << std::endl;
    std::cout << "LocalPlayer address: " << std::hex << localPlayer << std::dec << std::endl;
    std::cout << "Character address: " << std::hex << character << std::dec << std::endl;
    std::cout << "VisualEngine address: " << std::hex << visualEngine << std::dec << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    std::cout << "User ID: " << userID << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Team: " << teamName << std::endl;

    Gui gui;
    gui.Setup();

    //system("pause");
}