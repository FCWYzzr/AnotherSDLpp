//
// Created by FCWY on 25-3-12.
//

#ifndef SDL_AUDIO_HPP
#define SDL_AUDIO_HPP
#include <SDL3/SDL_audio.h>

namespace SDL::inline audio {

    namespace mask {
        constexpr auto size         =  SDL_AUDIO_MASK_BITSIZE;
        constexpr auto floating     =  SDL_AUDIO_MASK_FLOAT;
        constexpr auto big_endian   =  SDL_AUDIO_MASK_BIG_ENDIAN;
        constexpr auto with_sign    =  SDL_AUDIO_MASK_BITSIZE;
    }


    enum class Format {
        UNKNOWN = SDL_AUDIO_UNKNOWN,
        U8      = SDL_AUDIO_U8,
        S8      = SDL_AUDIO_S8,
        S16     = SDL_AUDIO_S16,
        S32     = SDL_AUDIO_S32,
        F32     = SDL_AUDIO_F32,
        S16LE   = SDL_AUDIO_S16LE,
        S16BE   = SDL_AUDIO_S16BE,
        S32LE   = SDL_AUDIO_S32LE,
        S32BE   = SDL_AUDIO_S32BE,
        F32LE   = SDL_AUDIO_F32LE,
        F32BE   = SDL_AUDIO_F32BE
    };

    constexpr Format make_format(const bool with_sign, const bool is_bigendian, const bool is_float, const unsigned char size) noexcept {
        return static_cast<Format>(SDL_DEFINE_AUDIO_FORMAT(with_sign, is_bigendian, is_float, size));
    }

    constexpr auto bitsize(const Format x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_BITSIZE;
    }

    constexpr auto bytesize(const Format x) {
        return bitsize(x) / 8;
    }

    constexpr bool is_float(const Format x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_FLOAT;
    }

    constexpr bool is_bigendian(const Format x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_BIG_ENDIAN;
    }

    constexpr bool is_littleendian(const Format x) {
        return !is_bigendian(x);
    }

    constexpr bool is_signed(const Format x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_BITSIZE;
    }

    enum class AudioDeviceID: Uint32{};

    constexpr auto default_playback = static_cast<AudioDeviceID>(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    constexpr auto default_recording = static_cast<AudioDeviceID>(SDL_AUDIO_DEVICE_DEFAULT_RECORDING);


}

namespace SDL::plus::inline audio {
    struct Spec {
        Format format;
        int channels;
        int freq;

        Spec(const Format format, const int channels, const int freq) noexcept:
            format{format},
            channels{channels},
            freq{freq} {}

        Spec(const SDL_AudioSpec& other) noexcept:
            format{other.format},
            channels{other.channels},
            freq{other.freq} {}
        operator SDL_AudioSpec() const noexcept {
            return {static_cast<SDL_AudioFormat>(static_cast<int>(format)), channels, freq};
        }

        constexpr auto frame_size() const noexcept {
            return bytesize(format) * channels;
        }
    };

    namespace Driver {
        inline int size() noexcept {
            return SDL_GetNumAudioDrivers();
        }

        inline const char* at(const int index) noexcept {
            return SDL_GetAudioDriver(index);
        }

        inline const char* current() noexcept {
            return SDL_GetCurrentAudioDriver();
        }
    };

    namespace Devices{
        struct Device {
            AudioDeviceID id;

            const char* name() {
                auto name = SDL_GetAudioDeviceName(static_cast<SDL_AudioDeviceID>(id));
                if (!name)
                    throw Error{};
                return name;
            }

            std::pair<Spec, int> format() {
                std::pair<SDL_AudioSpec, int> ret;
                if (!SDL_GetAudioDeviceFormat(
                    static_cast<SDL_AudioDeviceID>(id),
                    &ret.first,
                    &ret.second
                    ))
                    throw Error{};
                return ret;
            }
        };

        [[nodiscard]]
        inline List<Device>
            playback_devices() noexcept {
            int size;
            auto data = SDL_GetAudioPlaybackDevices(&size);
            if (!data)
                throw Error{};
            return {reinterpret_cast<Device*>(data), size};
        }

        [[nodiscard]]
        inline List<Device>
            recording_devices() noexcept {
            int size;
            auto data = SDL_GetAudioRecordingDevices(&size);
            if (!data)
                throw Error{};
            return {reinterpret_cast<Device*>(data), size};
        }
    }

    struct Stream {
        using handle_t = SDL_AudioStream*;
    };
}

#endif //SDL_AUDIO_HPP
