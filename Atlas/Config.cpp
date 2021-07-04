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

void Config::Save(char* configName, float& crosshairColor, float& fovCircleColor, float& espBoxColor, float& nameColor, float& distanceColor, float& headDotColor) {
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

    config["CrosshairColor"] = crosshairColor;
    config["FOVCircleColor"] = fovCircleColor;
    config["ESPBoxColor"] = espBoxColor;
    config["NameColor"] = nameColor;
    config["DistanceColor"] = distanceColor;
    config["HeadDotColor"] = headDotColor;

    saveFile("Configs\\" + std::string(configName) + ".txt", config.dump());
    MessageBoxA(0, (LPCSTR)("Saved config '" + std::string(configName) + "'").c_str(), "Saved Config", MB_OK);
}

void Config::Load(char* configName, float& crosshairColor, float& fovCircleColor, float& espBoxColor, float& nameColor, float& distanceColor, float& headDotColor) {
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

    config["CrosshairColor"].get_to(crosshairColor);
    config["ESPBoxColor"].get_to(espBoxColor);
    config["NameColor"].get_to(nameColor);
    config["DistanceColor"].get_to(distanceColor);
    config["HeadDotColor"].get_to(headDotColor);
}