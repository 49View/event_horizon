#pragma once

#include <string>
#include <memory>
#include <core/name_policy.hpp>

class AudioStream : public NamePolicy<> {

};

class AudioManager {
public:
	virtual std::shared_ptr<AudioStream> createStream( const std::string& streamName, const std::string& streamId ) = 0;
	virtual void playStream( const std::string& streamName ) = 0;
    virtual void update() = 0;
};


