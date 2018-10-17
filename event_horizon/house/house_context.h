#pragma once

#include <string>

class HouseContext {
public:
	static HouseContext& getInstance() {
		static HouseContext instance;
		return instance;
	}

	inline std::string HouseScope() const { return houseScope; }
	void HouseScope( std::string val ) { houseScope = val; }

	inline std::string ConfigurationScope() const { return configurationScope; }
	void ConfigurationScope( std::string val ) { configurationScope = val; }

private:
	HouseContext() {
		houseScope = "";
		configurationScope = "";
	}

	HouseContext( HouseContext const& ) = delete;
	void operator=( HouseContext const& ) = delete;
	~HouseContext() {}

private:
	std::string houseScope;
	std::string configurationScope;
};

#define CurrentHouseContext HouseContext::getInstance()