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

    std::uintptr_t processID = Memory::GetProcessID(L"RobloxPlayerBeta.exe");
    if (!processID) {
        std::cout << "Couldn't get Roblox process ID.\n";
        return 0;
    }

    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!handle) {
        std::cout << "Couldn't get Roblox handle.\n";
        return 0;
    }
    Globals::Handle = handle;

    HWND hwnd = FindWindowA(0, "Roblox");
    Globals::RobloxHWND = hwnd;

    __int64 userID = 0;
    std::string teamName;

    std::uintptr_t baseAddress = Memory::GetModuleBaseAddress(processID, L"RobloxPlayerBeta.exe");
    Globals::BaseAddress = baseAddress;

    std::cout << "Welcome to Atlas. Scanning...\n";

    DataModel game = Memory::Scan(baseAddress + Addresses::DataModel);
    Players players = Players(game.GetService("Players").Address);
    Player localPlayer = players.GetLocalPlayer();
    Instance character = localPlayer.GetCharacter();
    Instance team = localPlayer.GetTeam();

    VisualEngine visualEngine = Memory::Scan(baseAddress + Addresses::VisualEngine);

    Globals::PlayersService = players;
    Globals::LocalPlayer = localPlayer;
    Globals::VisualEngine = visualEngine;

    userID = localPlayer.GetUserID();

    if (team.Address) {
        teamName = team.GetName();
    } else {
        teamName = "None";
    }

    std::cout << "--------------------------------------\n";
    std::cout << "Base address: " << std::hex << baseAddress << '\n';
    std::cout << "DataModel address: " << game.Address << '\n';
    std::cout << "Players address: " << players.Address << '\n';
    std::cout << "LocalPlayer address: " << localPlayer.Address << '\n';
    std::cout << "Character address: " << character.Address << '\n';
    std::cout << "VisualEngine address: " << visualEngine.Address << std::dec << '\n';
    std::cout << "--------------------------------------\n";

    std::cout << "User ID: " << userID << '\n';
    std::cout << "Username: " << localPlayer.GetName() << '\n';
    std::cout << "Team: " << teamName << '\n';

    Gui gui;
    gui.Setup();

    //system("pause");
}