#pragma once
#include<string>

class flight {
public:
	flight();
	float distance{};
	bool weather_delay{};
	std::string origin_city;
};