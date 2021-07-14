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
        system("pause");
        return 0;
    }

    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!handle) {
        std::cout << "Couldn't get Roblox handle.\n";
        system("pause");
        return 0;
    }
    Globals::Handle = handle;

    HWND hwnd = FindWindowA(0, "Roblox");
    Globals::RobloxHWND = hwnd;

    std::string teamName;

    std::uintptr_t baseAddress = Memory::GetModuleBaseAddress(processID, L"RobloxPlayerBeta.exe");
    Globals::BaseAddress = baseAddress;

    std::cout << "Welcome to Atlas. Scanning...\n";

    Instance scriptContext = Memory::Scan(baseAddress + Addresses::ScriptContext); // Using ScriptContext to get the DataModel object because sometimes scanning for the DataModel VFTable leads to multiple results.
    DataModel game = static_cast<DataModel>(scriptContext.GetParent().Address);
    Players players = static_cast<Players>(game.GetService("Players").Address);
    Player localPlayer = players.GetLocalPlayer();
    Instance character = localPlayer.GetCharacter();
    Instance team = localPlayer.GetTeam();

    VisualEngine visualEngine = Memory::Scan(baseAddress + Addresses::VisualEngine);

    Globals::PlayersService = players;
    Globals::LocalPlayer = localPlayer;
    Globals::VisualEngine = visualEngine;

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

    std::cout << "Username: " << localPlayer.GetName() << '\n';
    std::cout << "Team: " << teamName << '\n';

    Gui gui;
    gui.Setup();

    //system("pause");
}