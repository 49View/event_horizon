//
// Created by Dado on 2019-02-06.
//

#pragma once

#include <graphics/audio/audio_manager.h>

class AudioManagerOpenAL : public AudioManager {
public:
    AudioManagerOpenAL();
    virtual ~AudioManagerOpenAL();

    std::shared_ptr<AudioStream> createStream( const std::string& streamName, const std::string& streamId ) override;
    void playStream( const std::string& streamName ) override;

    void update() override;
private:
    class AudioManagerOpenALImpl;
    std::unique_ptr<AudioManagerOpenALImpl> pimpl;
};



