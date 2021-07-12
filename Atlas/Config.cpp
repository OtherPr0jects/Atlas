#include <iostream>
#include <fstream>
#include "Config.h"
#include "Globals.h"
#include "json.hpp"

using json = nlohmann::json;

void replace(std::string& str, const std::string& from, const std::string& to) {
    std::size_t startPosition = str.find(from);
    if (startPosition == std::string::npos) return;
    str.replace(startPosition, from.length(), to);
}

std::string getExecutablePath() { // Location of the executable file. NOT the current working directory.
    char path[MAX_PATH];
    std::string stringPath = std::string(path, GetModuleFileNameA(0, path, sizeof(path)));
    replace(stringPath, "\\Atlas.exe", "");
    return stringPath;
}

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

template<typename T>
void loadConfigOption(T& globalSetting, const json& configObject, const std::string& configSetting) {
    if (!configObject.contains(configSetting)) return;
    globalSetting = configObject[configSetting];
}

void Config::Save(char* configName) {
    json config;

    config["ESPBoxes"] = Globals::ESPBoxEnabled;
    config["ESPTracers"] = Globals::ESPTracerEnabled;
    config["ESPNames"] = Globals::ESPNameEnabled;
    config["ESPDistances"] = Globals::ESPDistanceEnabled;
    config["ESPHeadDots"] = Globals::ESPHeadDotEnabled;
    config["HealthBarEnabled"] = Globals::HealthBarEnabled;

    config["TracerStartLocation"] = Globals::TracerStartLocation;

    config["Aimbot"] = Globals::AimbotEnabled;
    config["TeamCheck"] = Globals::TeamCheck;
    config["HealthCheck"] = Globals::HealthCheck;
    config["Crosshair"] = Globals::Crosshair;
    config["ViewFOVCircle"] = Globals::ViewFOVCircle;

    config["Target"] = Globals::Target;
    config["AimbotBind"] = Globals::AimbotBind;

    config["CrosshairScale"] = Globals::CrosshairScale;
    config["CrosshairThickness"] = Globals::CrosshairThickness;
    config["FOVSize"] = Globals::FOVSize;
    config["FOVCircleThickness"] = Globals::FOVCircleThickness;
    config["AimbotSmoothness"] = Globals::AimbotSmoothness;
    config["ESPBoxThickness"] = Globals::ESPBoxThickness;
    config["ESPTracerThickness"] = Globals::TracerThickness;
    config["HealthBarThickness"] = Globals::HealthBarThickness;

    config["RainbowSpeed"] = Globals::RainbowSpeed;

    config["CrosshairColor"] = Globals::CrosshairColor;
    config["FOVCircleColor"] = Globals::FOVCircleColor;
    config["ESPBoxColor"] = Globals::ESPBoxColor;
    config["TracerColor"] = Globals::TracerColor;
    config["NameColor"] = Globals::NameColor;
    config["DistanceColor"] = Globals::DistanceColor;
    config["HeadDotColor"] = Globals::HeadDotColor;

    saveFile(getExecutablePath() + "\\Configs\\" + static_cast<std::string>(configName) + ".txt", config.dump());
    MessageBoxA(
        0,
        static_cast<LPCSTR>(("Saved config '" + static_cast<std::string>(configName) + "'").c_str()),
        "Saved Config",
        MB_OK
    );
}

void Config::Load(char* configName) {
    std::string configJson = "";
    loadFile(getExecutablePath() + "\\Configs\\" + static_cast<std::string>(configName) + ".txt", configJson);
    if (!configJson[0]) {
        MessageBoxA(
            0,
            static_cast<LPCSTR>(("Failed to load config '" + static_cast<std::string>(configName) + "'").c_str()),
            "Failed to Load Config",
            MB_OK
        );
        return;
    }
    
    json config = json::parse(configJson);

    loadConfigOption<bool>(Globals::ESPBoxEnabled, config, "ESPBoxes");
    loadConfigOption<bool>(Globals::ESPTracerEnabled, config, "ESPTracers");
    loadConfigOption<bool>(Globals::ESPNameEnabled, config, "ESPNames");
    loadConfigOption<bool>(Globals::ESPDistanceEnabled, config, "ESPDistances");
    loadConfigOption<bool>(Globals::ESPHeadDotEnabled, config, "ESPHeadDots");
    loadConfigOption<bool>(Globals::HealthBarEnabled, config, "HealthBarEnabled");

    loadConfigOption<int>(Globals::TracerStartLocation, config, "TracerStartLocation");

    loadConfigOption<bool>(Globals::AimbotEnabled, config, "Aimbot");
    loadConfigOption<bool>(Globals::TeamCheck, config, "TeamCheck");
    loadConfigOption<bool>(Globals::HealthCheck, config, "HealthCheck");
    loadConfigOption<bool>(Globals::Crosshair, config, "Crosshair");
    loadConfigOption<bool>(Globals::ViewFOVCircle, config, "ViewFOVCircle");

    loadConfigOption<int>(Globals::Target, config, "Target");
    loadConfigOption<int>(Globals::AimbotBind, config, "AimbotBind");

    loadConfigOption<float>(Globals::CrosshairScale, config, "CrosshairScale");
    loadConfigOption<float>(Globals::CrosshairThickness, config, "CrosshairThickness");
    loadConfigOption<float>(Globals::FOVSize, config, "FOVSize");
    loadConfigOption<float>(Globals::FOVCircleThickness, config, "FOVCircleThickness");
    loadConfigOption<float>(Globals::AimbotSmoothness, config, "AimbotSmoothness");
    loadConfigOption<float>(Globals::ESPBoxThickness, config, "ESPBoxThickness");
    loadConfigOption<float>(Globals::TracerThickness, config, "ESPTracerThickness");
    loadConfigOption<float>(Globals::HealthBarThickness, config, "HealthBarThickness");

    loadConfigOption<float>(Globals::RainbowSpeed, config, "RainbowSpeed");

    if (config.contains("CrosshairColor")) config["CrosshairColor"].get_to(Globals::CrosshairColor);
    if (config.contains("ESPBoxColor")) config["ESPBoxColor"].get_to(Globals::ESPBoxColor);
    if (config.contains("TracerColor")) config["TracerColor"].get_to(Globals::TracerColor);
    if (config.contains("NameColor")) config["NameColor"].get_to(Globals::NameColor);
    if (config.contains("DistanceColor")) config["DistanceColor"].get_to(Globals::DistanceColor);
    if (config.contains("HeadDotColor")) config["HeadDotColor"].get_to(Globals::HeadDotColor);

    MessageBoxA(
        0,
        static_cast<LPCSTR>(("Loaded config '" + static_cast<std::string>(configName) + "'").c_str()),
        "Loaded Config",
        MB_OK
    );
}