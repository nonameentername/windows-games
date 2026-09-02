#ifndef WINDOWS_GAMES_COMPAT_MMSYSTEM_H
#define WINDOWS_GAMES_COMPAT_MMSYSTEM_H

#include <SDL.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

#define SND_ASYNC 0x0001
#define SND_NODEFAULT 0x0002
#define SND_LOOP 0x0008

struct WindowsGamesPlayingSound
{
    unsigned int id;
    std::vector<Uint8> data;
    Uint32 position;
    bool loop;
};

inline std::vector<WindowsGamesPlayingSound> &windows_games_playing_sounds()
{
    static std::vector<WindowsGamesPlayingSound> sounds;
    return sounds;
}

inline SDL_AudioDeviceID &windows_games_audio_device()
{
    static SDL_AudioDeviceID device = 0;
    return device;
}

inline SDL_AudioSpec &windows_games_audio_spec()
{
    static SDL_AudioSpec spec;
    return spec;
}

inline unsigned int windows_games_next_sound_id()
{
    static unsigned int next_id = 1;
    return next_id++;
}

inline void windows_games_audio_callback(void *, Uint8 *stream, int len)
{
    SDL_memset(stream, 0, len);

    std::vector<WindowsGamesPlayingSound> &sounds = windows_games_playing_sounds();
    for (std::vector<WindowsGamesPlayingSound>::iterator it = sounds.begin(); it != sounds.end();) {
        Uint32 remaining = static_cast<Uint32>(it->data.size()) - it->position;
        Uint32 amount = std::min<Uint32>(remaining, static_cast<Uint32>(len));

        if (amount > 0)
            SDL_MixAudioFormat(stream, it->data.data() + it->position, windows_games_audio_spec().format, amount, SDL_MIX_MAXVOLUME);

        it->position += amount;
        if (it->position >= it->data.size()) {
            if (it->loop) {
                it->position = 0;
                ++it;
            } else {
                it = sounds.erase(it);
            }
        } else {
            ++it;
        }
    }
}

inline bool windows_games_init_audio()
{
    if (windows_games_audio_device() != 0)
        return true;

    if (SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return false;

    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 2048;
    desired.callback = windows_games_audio_callback;

    SDL_AudioSpec obtained;
    SDL_zero(obtained);
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (device == 0)
        return false;

    windows_games_audio_spec() = obtained;
    windows_games_audio_device() = device;
    SDL_PauseAudioDevice(device, 0);
    return true;
}

inline bool windows_games_convert_wav(const char *path, std::vector<Uint8> &data)
{
    SDL_AudioSpec wav_spec;
    Uint8 *wav_buffer = nullptr;
    Uint32 wav_length = 0;

    if (SDL_LoadWAV(path, &wav_spec, &wav_buffer, &wav_length) == nullptr)
        return false;

    SDL_AudioSpec &device_spec = windows_games_audio_spec();
    SDL_AudioCVT converter;
    if (SDL_BuildAudioCVT(&converter,
                          wav_spec.format, wav_spec.channels, wav_spec.freq,
                          device_spec.format, device_spec.channels, device_spec.freq) < 0) {
        SDL_FreeWAV(wav_buffer);
        return false;
    }

    converter.len = static_cast<int>(wav_length);
    converter.buf = static_cast<Uint8 *>(SDL_malloc(wav_length * converter.len_mult));
    if (converter.buf == nullptr) {
        SDL_FreeWAV(wav_buffer);
        return false;
    }

    SDL_memcpy(converter.buf, wav_buffer, wav_length);
    SDL_FreeWAV(wav_buffer);

    if (SDL_ConvertAudio(&converter) < 0) {
        SDL_free(converter.buf);
        return false;
    }

    data.assign(converter.buf, converter.buf + converter.len_cvt);
    SDL_free(converter.buf);
    return true;
}

inline bool windows_games_load_sound(const char *file, std::vector<Uint8> &data)
{
    std::vector<std::string> candidates;
    candidates.push_back(file);

    char *base_path = SDL_GetBasePath();
    if (base_path != nullptr) {
        candidates.push_back(std::string(base_path) + file);
        SDL_free(base_path);
    }

    candidates.push_back(std::string("celda/") + file);
    candidates.push_back(std::string("battleship/") + file);

    for (std::vector<std::string>::const_iterator it = candidates.begin(); it != candidates.end(); ++it) {
        if (windows_games_convert_wav(it->c_str(), data))
            return true;
    }

    return false;
}

inline bool windows_games_sound_is_playing(unsigned int id)
{
    SDL_AudioDeviceID device = windows_games_audio_device();
    if (device == 0)
        return false;

    SDL_LockAudioDevice(device);
    const std::vector<WindowsGamesPlayingSound> &sounds = windows_games_playing_sounds();
    bool playing = std::find_if(sounds.begin(), sounds.end(), [id](const WindowsGamesPlayingSound &sound) {
        return sound.id == id;
    }) != sounds.end();
    SDL_UnlockAudioDevice(device);
    return playing;
}

inline int windows_games_snd_play_sound(const char *file, unsigned int flags)
{
    if (file == nullptr) {
        SDL_AudioDeviceID device = windows_games_audio_device();
        if (device != 0) {
            SDL_LockAudioDevice(device);
            windows_games_playing_sounds().clear();
            SDL_UnlockAudioDevice(device);
        }
        return 1;
    }

    if (!windows_games_init_audio())
        return 0;

    std::vector<Uint8> data;
    if (!windows_games_load_sound(file, data))
        return 0;

    SDL_AudioDeviceID device = windows_games_audio_device();
    unsigned int id = windows_games_next_sound_id();

    WindowsGamesPlayingSound sound;
    sound.id = id;
    sound.data.swap(data);
    sound.position = 0;
    sound.loop = (flags & SND_LOOP) != 0;

    SDL_LockAudioDevice(device);
    windows_games_playing_sounds().clear();
    windows_games_playing_sounds().push_back(sound);
    SDL_UnlockAudioDevice(device);

    if ((flags & SND_ASYNC) == 0) {
        while (windows_games_sound_is_playing(id))
            SDL_Delay(10);
    }

    return 1;
}

#define sndPlaySound(file, flags) windows_games_snd_play_sound((file), static_cast<unsigned int>(flags))
#define PlaySound(file, module, flags) windows_games_snd_play_sound((file), static_cast<unsigned int>(flags))

#endif
