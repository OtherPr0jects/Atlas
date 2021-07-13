#include <thread>
#include <iostream>
#include "Utility.h"

union HexToFloat {
    std::uintptr_t Hex;
    float Float;
};

std::string Utility::CreateRandomString(int length) {
    std::string randomString;

    char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned)time(NULL));

    randomString.reserve(length);

    for (int i = 0; i < length; ++i)
        randomString += alphanum[rand() % (sizeof(alphanum) - 1)];

    return randomString;
}

// Arguments should be 0-1. Returns RGB in 0-1.
void Utility::HSVToRGB(float h, float s, float v, float rgbArray[4]) {
    h = h * 360;
    float c = s * v;
    float x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r, g, b;

    if (h >= 0 && h < 60) {
        r = c, g = x, b = 0;
    } else if (h >= 60 && h < 120) {
        r = x, g = c, b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0, g = c, b = x;
    } else if (h >= 180 && h < 240) {
        r = 0, g = x, b = c;
    } else if (h >= 240 && h < 300) {
        r = x, g = 0, b = c;
    } else {
        r = c, g = 0, b = x;
    }

    r += m;
    g += m;
    b += m;

    rgbArray[0] = r;
    rgbArray[1] = g;
    rgbArray[2] = b;
}

float Utility::HexToFloat(std::uintptr_t hex) {
    union HexToFloat conversion;
    conversion.Hex = hex;
    return conversion.Float;
}