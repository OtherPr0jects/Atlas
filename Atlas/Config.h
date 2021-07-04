#pragma once

class Config {
	public:
		static void Save(
			char* configName, float& crosshairColor, float& fovCircleColor,
			float& espBoxColor, float& nameColor, float& distanceColor,
			float& headDotColor
		);
		static void Load(
			char* configName, float& crosshairColor, float& fovCircleColor,
			float& espBoxColor, float& nameColor, float& distanceColor,
			float& headDotColor
		);
};