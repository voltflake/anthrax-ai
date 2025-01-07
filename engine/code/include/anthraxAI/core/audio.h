#pragma once
#include "anthraxAI/utils/defines.h"

#include "AL/al.h"
#include "AL/alext.h"

namespace Core
{
    class Audio : public Utils::Singleton<Audio>
    {
        public:
            Audio() {}
            ~Audio() { ALCboolean closed = alcCloseDevice(Device); }
            
            void Init();
            void Load(const std::string& name);
            void Play();

            const std::vector<std::string>& GetAudioNames() const { return AudioNames; }
            const std::string& GetCurrentSound() const { return CurrentSound; }
            void ResetState() { State = AL_STOPPED; }
            void SetState(bool state);//{ if (state) { State = AL_PLAYING; } else { State = AL_PAUSED; } }
        private:
            ALCdevice* Device = nullptr;
            ALCcontext* Context = nullptr;
            ALCboolean EAXSupport = false;
            ALuint Buffer;
            ALint State;
            ALuint Source;
            std::string CurrentSound;

            std::vector<std::string> AudioNames;

            void Release();
            bool LoadWAV(const std::string& name, std::uint8_t& channels, std::int32_t& sample_rate, std::uint8_t& bits_per_sample, std::vector<char>& sound_data);

    };

}