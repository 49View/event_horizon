#pragma once

#include <string>
#include <map>

class AudioManager {
public:
	static AudioManager& getInstance() {
		static AudioManager instance; // Guaranteed to be destroyed.
		return instance;// Instantiated on first use.
	}
private:
	AudioManager() {}
	AudioManager( AudioManager const& ) = delete;
	void operator=( AudioManager const& ) = delete;

public:
	void init();
	void createStream( const std::string& streamName );
	void playStream( const std::string& streamName );

private:
};

#define AM AudioManager::getInstance()

