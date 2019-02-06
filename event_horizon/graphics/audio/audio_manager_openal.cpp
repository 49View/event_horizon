//
// Created by Dado on 2019-02-06.
//

#include "audio_manager_openal.hpp"

#include <unordered_map>
#include <core/util.h>
#include <graphics/audio/audio_file.h>

#ifdef OSX
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#ifdef _ULTRADEBUG
#define ALCALL(X) do { X; checkAlError(#X, __LINE__, __FILE__); } while (0)
#else
#define ALCALL(X) X
#endif

const char *getAlErrorString( ALint error ) {
    switch ( error ) {
        case AL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case AL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case AL_NO_ERROR:
            return "GL_NO_ERROR";
        case AL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case AL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
    }
    return "Unknown AL Error";
}

void checkAlError( const char *op, int line, const char *file ) {
    ALint error = alGetError();
    if ( error != AL_NO_ERROR ) {
        LOGE( "OpenAL Error: %s - %s, (%s, %d)", op, getAlErrorString( error ), file, line );
    }
}

AudioManagerOpenAL::AudioManagerOpenAL() {
    pimpl = std::make_unique<AudioManagerOpenAL::AudioManagerOpenALImpl>();
}

static constexpr ALuint numMaxSources = 16;

class AudioStreamOpenAL : public AudioStream {
public:
    AudioStreamOpenAL( ALuint bufferId, const std::string& _name ) : bufferId( bufferId ) {
        Name(_name);
    }

    ALuint bufferId;
};

class AudioManagerOpenAL::AudioManagerOpenALImpl {
public:

    AudioManagerOpenALImpl() {
        device = alcOpenDevice(NULL);
        if (!device) {
            LOGR("Audio could not be initialized");
            return;
        }
        ALboolean enumeration;

        enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
        if (enumeration == AL_FALSE) {
            LOGR("Audio devices could not be enumerated");
            return;
        } else {
            list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
        }

        ALCcontext *context;

        context = alcCreateContext(device, NULL);
        if (!alcMakeContextCurrent(context)) {
            LOGR("Audio create context failed");
        }

        ALCALL(alGenSources(numMaxSources, sources));
    }

    void list_audio_devices(const ALCchar *devices)
    {
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;
        std::string edevices;

        while (device && *device != '\0' && next && *next != '\0') {
            edevices += std::string(device) + " ";
            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
        }
        LOGR( "OpenAL devices: %s", edevices.c_str() );
    }

    ALenum to_al_format(short channels, short samples)
    {
        bool stereo = (channels > 1);

        switch (samples) {
            case 16:
                if (stereo)
                    return AL_FORMAT_STEREO16;
                else
                    return AL_FORMAT_MONO16;
            case 8:
                if (stereo)
                    return AL_FORMAT_STEREO8;
                else
                    return AL_FORMAT_MONO8;
            default:
                return -1;
        }
    }

    std::shared_ptr<AudioStream> createStream( const std::string& _streamName, const std::string& _streamId ) {
        ALuint buffer;
        ALCALL(alGenBuffers((ALuint)1, &buffer));

        AudioFile<float> audioFile;
        bool bOk = audioFile.load(_streamName);

        if ( !bOk ) {
            LOGR("[OpenAL] cannot create stream form %s", _streamName.c_str());
            return nullptr;
        }

        std::vector<int8_t>  eib;
        auto sr = audioFile.getSampleRate();
        auto sr2 = sr / 2;
        for ( size_t i = 0; i < audioFile.samples[0].size(); i++ ) {
            for ( size_t c = 0; c < audioFile.getNumChannels(); c++ ) {
                auto s = audioFile.samples[c][i];
                auto ss = ((( s * sr2 - sr2 ) / sr2 ) * 128.0f );
                eib.emplace_back( static_cast<int8_t>(ss));
            }
        }

        ALCALL(alBufferData(buffer, to_al_format(audioFile.getNumChannels(), audioFile.getBitDepth()),
                     eib.data(),eib.size(),audioFile.getSampleRate()));

        auto stream = std::make_shared<AudioStreamOpenAL>(buffer, _streamName);
        streams.emplace(_streamId, stream);
        return stream;
    }

    void playStream( const std::string& _streamId, ALuint _ch = 0 ) {

        if ( auto it = streams.find(_streamId); it != streams.end() ) {
            auto stream = it->second;
            ALCALL(alSourcei(sources[_ch], AL_BUFFER, stream->bufferId));
            ALCALL(alSourcePlay(sources[_ch]));
        }

    }

    void update() {
//        ALint source_state = 0;
//        ALCALL(alGetSourcei(sources[_ch], AL_SOURCE_STATE, &source_state));
//        while (source_state == AL_PLAYING) {
//            ALCALL(alGetSourcei(sources[_ch], AL_SOURCE_STATE, &source_state));
//        }
    }

private:
    ALCdevice *device = nullptr;
    ALuint sources[numMaxSources];
    std::unordered_map<std::string, std::shared_ptr<AudioStreamOpenAL>> streams;
};

std::shared_ptr<AudioStream> AudioManagerOpenAL::createStream( const std::string& streamName, const std::string& _streamId ) {
    return pimpl->createStream( streamName, _streamId );
}

void AudioManagerOpenAL::playStream( const std::string& streamName ) {
    pimpl->playStream( streamName );
}

void AudioManagerOpenAL::update() {
    pimpl->update();
}


AudioManagerOpenAL::~AudioManagerOpenAL() = default;
