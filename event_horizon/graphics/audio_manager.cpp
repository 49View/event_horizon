#include "audio_manager.h"

void AudioManager::init() {
//	FMOD_RESULT       result;
//	void             *extradriverdata = 0;
//
//	result = FMOD::System_Create( &system );
//	result = system->init( 32, FMOD_INIT_NORMAL, extradriverdata );
}

void AudioManager::createStream( [[maybe_unused]] const std::string& streamName ) {
//	FMOD_RESULT       result;
//	FMOD::Sound      *sound;
//
//	result = system->createStream( ( std::string( "data/" ) + streamName ).c_str(), FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound );
//	streams.insert( std::make_pair( streamName, sound ) );
}

void AudioManager::playStream( [[maybe_unused]] const std::string& streamName ) {
//	FMOD_RESULT       result;
//	FMOD::Channel    *channel = 0;
//	FMOD::Sound      *sound = streams[streamName];
//
//	result = system->playSound( sound, 0, false, &channel );
}