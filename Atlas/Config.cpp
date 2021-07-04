#include <iostream>
#include <fstream>
#include "Config.h"
#include "Globals.h"
#include "json.hpp"

using json = nlohmann::json;

void saveFile(const std::string& filePath, const std::string& data) {
    std::ofstream outFile;
    outFile.open(filePath);
    outFile << data;
    outFile.close();
}

void loadFile(const std::string& filePath, std::string& data) {
    std::ifstream inFile;
    inFile.open(filePath);
    inFile >> data;
    inFile.close();
}

void Config::Save(char* configName) {
    json config;

    config["ESPBoxes"] = Globals::ESPBoxEnabled;
    config["ESPNames"] = Globals::ESPNameEnabled;
    config["ESPDistances"] = Globals::ESPDistanceEnabled;

    config["Aimbot"] = Globals::ESPBoxEnabled;
    config["TeamCheck"] = Globals::TeamCheck;
    config["Crosshair"] = Globals::Crosshair;
    config["ViewFOVCircle"] = Globals::ViewFOVCircle;

    config["Target"] = Globals::Target;
    config["AimbotBind"] = Globals::AimbotBind;

    config["CrosshairScale"] = Globals::CrosshairScale;
    config["CrosshairThickness"] = Globals::CrosshairThickness;
    config["FOVSize"] = Globals::FOVSize;
    config["AimbotSmoothness"] = Globals::AimbotSmoothness;
    config["ESPBoxThickness"] = Globals::ESPBoxThickness;

    config["CrosshairColor"] = Globals::CrosshairColor;
    config["FOVCircleColor"] = Globals::FOVCircleColor;
    config["ESPBoxColor"] = Globals::ESPBoxColor;
    config["NameColor"] = Globals::NameColor;
    config["DistanceColor"] = Globals::DistanceColor;
    config["HeadDotColor"] = Globals::HeadDotColor;

    saveFile("Configs\\" + std::string(configName) + ".txt", config.dump());
    MessageBoxA(0, (LPCSTR)("Saved config '" + std::string(configName) + "'").c_str(), "Saved Config", MB_OK);
}

void Config::Load(char* configName) {
    std::string configJson = "";
    loadFile("Configs\\" + std::string(configName) + ".txt", configJson);
    if (!configJson[0]) {
        MessageBoxA(0, (LPCSTR)("Failed to load config '" + std::string(configName) + "'").c_str(), "Failed to Load Config", MB_OK);
        return;
    }
    
    json config = json::parse(configJson);

    Globals::ESPBoxEnabled = config["ESPBoxes"];
    Globals::ESPNameEnabled = config["ESPNames"];
    Globals::ESPDistanceEnabled = config["ESPDistances"];

    Globals::ESPBoxEnabled = config["Aimbot"];
    Globals::TeamCheck = config["TeamCheck"];
    Globals::Crosshair = config["Crosshair"];
    Globals::ViewFOVCircle = config["ViewFOVCircle"];

    Globals::Target = config["Target"];
    Globals::AimbotBind = config["AimbotBind"];

    Globals::CrosshairScale = config["CrosshairScale"];
    Globals::CrosshairThickness = config["CrosshairThickness"];
    Globals::FOVSize = config["FOVSize"];
    Globals::AimbotSmoothness = config["AimbotSmoothness"];
    Globals::ESPBoxThickness = config["ESPBoxThickness"];

    config["CrosshairColor"].get_to(Globals::CrosshairColor);
    config["ESPBoxColor"].get_to(Globals::ESPBoxColor);
    config["NameColor"].get_to(Globals::NameColor);
    config["DistanceColor"].get_to(Globals::DistanceColor);
    config["HeadDotColor"].get_to(Globals::HeadDotColor);

    MessageBoxA(0, (LPCSTR)("Loaded config '" + std::string(configName) + "'").c_str(), "Loaded Config", MB_OK);
}