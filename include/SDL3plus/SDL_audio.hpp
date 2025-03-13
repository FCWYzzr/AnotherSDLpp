//
// Created by FCWY on 25-3-12.
//

#ifndef SDL_AUDIO_HPP
#define SDL_AUDIO_HPP
#include <SDL3/SDL_audio.h>
#include "SDL_stdinc.hpp"

#include "SDL_endian.hpp"
#include "SDL_error.hpp"
#include "SDL_mutex.hpp"
#include "SDL_properties.hpp"
#include "SDL_iostream.hpp"
#include <ranges>

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

    enum class DeviceID: Uint32{};

    constexpr auto default_playback = static_cast<DeviceID>(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    constexpr auto default_recording = static_cast<DeviceID>(SDL_AUDIO_DEVICE_DEFAULT_RECORDING);


}

namespace SDL::plus::inline audio {
    struct Spec {
        Format format;
        int channels;
        int freq;

        Spec()=default;

        Spec(const Format format, const int channels, const int freq) noexcept:
            format{format},
            channels{channels},
            freq{freq} {}

        Spec(const SDL_AudioSpec& other) noexcept:
            format{other.format},
            channels{other.channels},
            freq{other.freq} {}

        operator SDL_AudioSpec& () & noexcept {
            return reinterpret_cast<SDL_AudioSpec&>(*this);
        }

        operator SDL_AudioSpec () && noexcept {
            return {static_cast<SDL_AudioFormat>(static_cast<int>(format)), channels, freq};
        }

        constexpr auto frame_size() const noexcept {
            return bytesize(format) * channels;
        }

        auto legacy_addr() const noexcept {
            return reinterpret_cast<const SDL_AudioSpec*>(this);
        }
        auto legacy_addr() noexcept {
            return reinterpret_cast<SDL_AudioSpec*>(this);
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

    struct Device {
        DeviceID id;

        // this id will NOT promise to be the member
        // return value of SDL_OpenAudioDevice will be used
        Device(DeviceID id, const Spec& spec):
            id{SDL_OpenAudioDevice(legacy_id(), reinterpret_cast<const SDL_AudioSpec*>(&spec))}{
            if (static_cast<Uint32>(id) == 0)
                throw Error{};
        }

        explicit Device(DeviceID id):
            id{SDL_OpenAudioDevice(legacy_id(), nullptr)}{
            if (static_cast<Uint32>(id) == 0)
                throw Error{};
        }

        ~Device() noexcept {
            SDL_CloseAudioDevice(legacy_id());
        }

        Device duplicate() const {
            return Device{id};
        }

        Device duplicate(const Spec& spec) const {
            return Device{id, spec};
        }


        const char* name() const {
            auto name = SDL_GetAudioDeviceName(legacy_id());
            if (!name)
                throw Error{};
            return name;
        }

        std::pair<Spec, int> format() const {
            std::pair<Spec, int> ret;
            if (!SDL_GetAudioDeviceFormat(
                legacy_id(),
                reinterpret_cast<SDL_AudioSpec*>(&ret.first),
                &ret.second
                ))
                throw Error{};
            return ret;
        }

        List<int> channel_map() const {
            int size;
            auto data = SDL_GetAudioDeviceChannelMap(legacy_id(), &size);
            return {data, size};
        }

        bool is_physical() const noexcept {
            return SDL_IsAudioDevicePhysical(legacy_id());
        }

        bool is_playback() const noexcept {
            return SDL_IsAudioDevicePlayback(legacy_id());
        }

        bool paused() const noexcept {

            return SDL_AudioDevicePaused(legacy_id());
        }

        float gain() const noexcept {
            if (const auto ret = SDL_GetAudioDeviceGain(legacy_id()) == -1.0f)
                throw Error{};
            else
                return ret;
        }

        constexpr SDL_AudioDeviceID
            legacy_id() const noexcept {
            return static_cast<SDL_AudioDeviceID>(id);
        }

        // ReSharper disable CppMemberFunctionMayBeConst
        void pause() {
            if (!SDL_PauseAudioDevice(legacy_id()))
                throw Error{};
        }

        void resume() {
            if (!SDL_ResumeAudioDevice(legacy_id()))
                throw Error{};
        }

        void set_gain(const float gain) noexcept {
            if (!SDL_SetAudioDeviceGain(legacy_id(), gain))
                throw Error{};
        }

        // ReSharper restore CppMemberFunctionMayBeConst



            

        template<typename... Args>
        static Device open_default_playback(Args&& arg) {
            return Device{default_playback, std::forward<Args>(arg)...};
        }

        template<typename... Args>
        static Device open_default_recording(Args&& arg) {
            return Device{default_recording, std::forward<Args>(arg)...};
        }
        [[nodiscard]]
        static List<Device>
            playback_devices() noexcept {
            int size;
            auto data = SDL_GetAudioPlaybackDevices(&size);
            if (!data)
                throw Error{};
            return {reinterpret_cast<Device*>(data), size};
        }

        [[nodiscard]]
        static List<Device>
            recording_devices() noexcept {
            int size;
            auto data = SDL_GetAudioRecordingDevices(&size);
            if (!data)
                throw Error{};
            return {reinterpret_cast<Device*>(data), size};
        }
    };

    struct Stream {
        using handle_t = SDL_AudioStream*;

        handle_t
            handle;

        Stream(const Spec& from, const Spec& to):
            handle{SDL_CreateAudioStream(from.legacy_addr(), to.legacy_addr())} {
            if (!handle)
                throw Error{};
        }

        // ReSharper disable CppMemberFunctionMayBeConst
        void bind(const Device& device) {
            if (!SDL_BindAudioStream(device.legacy_id(), handle))
                throw Error{};
        }

        void rebind(const Device& device) {
            unbind();
            bind(device);
        }

        void unbind() noexcept {
            return SDL_UnbindAudioStream(handle);
        }
        // ReSharper restore CppMemberFunctionMayBeConst

        DeviceID binding() const noexcept {
            return static_cast<DeviceID>(SDL_GetAudioStreamDevice(handle));
        }


        template<std::ranges::range Range>
        static void unbind(const Range& streams) noexcept{
            auto cache = std::vector<SDL_AudioStream*>{};
            for (Stream& e: streams)
                cache.emplace_back(e.handle);

            SDL_UnbindAudioStreams(cache.data(), cache.size());
        }


        template<std::ranges::range Range>
        static void bind(const Device& device, const Range& streams) {
            auto cache = std::vector<SDL_AudioStream*>{};
            for (Stream& e: streams)
                cache.emplace_back(e.handle);

            if (!SDL_BindAudioStreams(device.legacy_id(), cache.data(), cache.size()))
                throw Error{};
        }

        
    };


}

#endif //SDL_AUDIO_HPP
