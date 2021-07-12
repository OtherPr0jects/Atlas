#pragma once
#include <string>

class Utility {
	public:
		static std::string CreateRandomString(int length);
		static void HSVToRGB(float h, float s, float v, float rgbArray[4]);
		static float HexToFloat(std::uintptr_t hex);
};