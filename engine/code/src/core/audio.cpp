#include "anthraxAI/core/audio.h"
#include "anthraxAI/engine.h"
#include "anthraxAI/utils/debug.h"

#include <fstream>
void Core::Audio::Init()
{
    if (!Device) {
        Device = alcOpenDevice(nullptr);
    }
    if (Device) {
        Context = alcCreateContext(Device, nullptr);
        alcMakeContextCurrent(Context);
    }
    ASSERT(!Device, "Core::Audio::Init(): Device was not created!");
    ASSERT(!Context, "Core::Audio::Init(): Context was not created!");

    EAXSupport = alIsExtensionPresent("EAX2.0");

    if (AudioNames.empty()) {
        AudioNames.reserve(2);
        AudioNames.push_back("Anthrax_Mastered.wav");
        AudioNames.push_back("astral-creepy.wav");
    }
}

int32_t convert_to_int(char* buffer, size_t len)
{
    int32_t a = 0;
    if(std::endian::native == std::endian::little)
        memcpy(&a, buffer, len);
    else
        for(std::size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

bool Core::Audio::LoadWAV(const std::string& name, std::uint8_t& channels, std::int32_t& sample_rate, std::uint8_t& bits_per_sample, std::vector<char>& sound_data)
{
    std::ifstream file(name, std::ios::binary);
    ASSERT(!file.is_open(), "Core::Audio::LoadWAV(): Audio file error!");

    ALsizei size;
    char buffer[4];

    // RIFF
    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read RIFF");
    ASSERT(strncmp(buffer, "RIFF", 4) != 0, "Core::Audio::LoadWAV(): file is not a valid WAVE file (RIFF)");

    // size of file 
    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read size of file");

    // WAVE 
    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read WAVE");
    ASSERT(strncmp(buffer, "WAVE", 4) != 0, "Core::Audio::LoadWAV(): file is not a valid WAVE file (WAVE)");

    // fmt/0
    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read fmt/0");
    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read subchunk size (16 for PCM)");
    
    ASSERT(!file.read(buffer, 2), "Core::Audio::LoadWAV(): could not read audio format (1 for PCM -> liniar quantization)");

    ASSERT(!file.read(buffer, 2), "Core::Audio::LoadWAV(): could not read nums of channels");
    channels = convert_to_int(buffer, 2);

    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read sample rate");
    sample_rate = convert_to_int(buffer, 4);

    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read byte rate");

    ASSERT(!file.read(buffer, 2), "Core::Audio::LoadWAV(): could not read data for block allign");

    ASSERT(!file.read(buffer, 2), "Core::Audio::LoadWAV(): could not read bits per sample");
    bits_per_sample = convert_to_int(buffer, 2);

    // data subchunk
    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read data header");

    ASSERT(strncmp(buffer, "data", 4) != 0, "Core::Audio::LoadWAV(): file is not a valid WAVE file (data chunk)");

    ASSERT(!file.read(buffer, 4), "Core::Audio::LoadWAV(): could not read size of data");
    size = convert_to_int(buffer, 4);

    ASSERT(file.eof(), "Core::Audio::LoadWAV(): EOF?!");

    char* c = new char[size];
    file.read(c, size);

    sound_data.resize(size);
    for (int i = 0; i < size; i++) {
        sound_data[i] = c[i];
    }

    return true;    
}

void Core::Audio::Load(const std::string& name)
{
    if (State == AL_PLAYING) {
        Release();
        Init();
    }
    CurrentSound = name;

    std::uint8_t channels;
    std::int32_t sample_rate;
    std::uint8_t bits_per_sample;
    std::vector<char> sound_data;

    bool res = LoadWAV(name, channels, sample_rate, bits_per_sample, sound_data);
    ASSERT(!res, "Core::Audio::Load(): Failed to load WAV!");
   
    alGetError();
    alGenBuffers(1, &Buffer);
    ALenum err = alGetError();
    ASSERT(err == AL_NO_ERROR, "Core::Audio::Init(): Buffers error!");

    ALenum format;
    if(channels == 1 && bits_per_sample == 8) {
        format = AL_FORMAT_MONO8;
    }
    else if(channels == 1 && bits_per_sample == 16) {
        format = AL_FORMAT_MONO16;
    }
    else if(channels == 2 && bits_per_sample == 8) {
        format = AL_FORMAT_STEREO8;
    }
    else if(channels == 2 && bits_per_sample == 16) {
        format = AL_FORMAT_STEREO16;
    }
    else {
        ASSERT(true, "Core::Audio::Load(): Unkonw file format!");
    }

    alBufferData(Buffer, format, sound_data.data(), sound_data.size(), sample_rate);
    sound_data.clear();

    alGenSources(1, &Source);
    alSourcef( Source, AL_PITCH, 1);
    alSourcef( Source, AL_GAIN, 1.0f);
    alSource3f( Source, AL_POSITION, 0, 0, 0);
    alSource3f( Source, AL_VELOCITY, 0, 0, 0);
    alSourcei(Source, AL_LOOPING, AL_FALSE);
    alSourcei(Source, AL_BUFFER, Buffer);

    alSourcePlay(Source);

    State = AL_PLAYING;
}
void Core::Audio::SetState(bool state) 
{ 
    if (state) {
        if (State != AL_PLAYING) {
           alSourcePlay(Source);
        }
        State = AL_PLAYING;
    } 
    else { 
        State = AL_PAUSED;
        alSourcePause(Source);
    } 
}

void Core::Audio::Release()
{
        alDeleteSources(1, &Source);
        alDeleteBuffers(1, &Buffer);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(Context);
}

void Core::Audio::Play()
{
    if (State == AL_PLAYING) {
        alSourcef(Source, AL_GAIN, 0.2f);
        alGetSourcei(Source, AL_SOURCE_STATE, &State);

    }
    else if (State == AL_PAUSED) {
        //alGetSourcei(Source, AL_SOURCE_STATE, &State);
    }
    else {
        if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_INTRO)) {
            Engine::GetInstance()->CheckState();
        }
        Release();
    }
}
