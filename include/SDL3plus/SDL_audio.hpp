//
// Created by FCWY on 25-3-12.
//
// ReSharper disable CppMemberFunctionMayBeConst
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

namespace SDL{
    namespace audio_mask {
        constexpr auto size         =  SDL_AUDIO_MASK_BITSIZE;
        constexpr auto floating     =  SDL_AUDIO_MASK_FLOAT;
        constexpr auto big_endian   =  SDL_AUDIO_MASK_BIG_ENDIAN;
        constexpr auto with_sign    =  SDL_AUDIO_MASK_BITSIZE;
    }


    enum class AudioFormat {
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
    constexpr SDL_AudioFormat legacy(const AudioFormat format) noexcept {
        return static_cast<SDL_AudioFormat>(format);
    }

    inline std::string_view to_string(const AudioFormat format) {
        return SDL_GetAudioFormatName(static_cast<SDL_AudioFormat>(format));
    }

    inline int silent_value(const AudioFormat format) {
        return SDL_GetSilenceValueForFormat(legacy(format));
    }

    constexpr AudioFormat make_format(const bool with_sign, const bool is_bigendian, const bool is_float, const unsigned char size) noexcept {
        return static_cast<AudioFormat>(SDL_DEFINE_AUDIO_FORMAT(with_sign, is_bigendian, is_float, size));
    }

    constexpr auto bitsize(const AudioFormat x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_BITSIZE;
    }

    constexpr auto bytesize(const AudioFormat x) {
        return bitsize(x) / 8;
    }

    constexpr bool is_float(const AudioFormat x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_FLOAT;
    }

    constexpr bool is_bigendian(const AudioFormat x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_BIG_ENDIAN;
    }

    constexpr bool is_littleendian(const AudioFormat x) {
        return !is_bigendian(x);
    }

    constexpr bool is_signed(const AudioFormat x) {
        return static_cast<Uint16>(x) & SDL_AUDIO_MASK_BITSIZE;
    }

    enum class AudioDeviceID: Uint32{};
    constexpr SDL_AudioDeviceID legacy(const AudioDeviceID device) noexcept {
        return static_cast<SDL_AudioDeviceID>(device);
    }

    constexpr auto default_playback = static_cast<AudioDeviceID>(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    constexpr auto default_recording = static_cast<AudioDeviceID>(SDL_AUDIO_DEVICE_DEFAULT_RECORDING);

    struct AudioSpec {
        AudioFormat format;
        int channels;
        int freq;

        AudioSpec()=default;

        AudioSpec(const AudioFormat format, const int channels, const int freq) noexcept:
            format{format},
            channels{channels},
            freq{freq} {}

        AudioSpec(const SDL_AudioSpec& other) noexcept:
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

    namespace AudioDriver {
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

    struct AudioDevice;

    struct AudioStream {
        using handle_t = SDL_AudioStream*;

        handle_t
            handle;

        explicit AudioStream(handle_t handle):
            handle{handle} {
            if (!handle)
                throw Error{};
        }

        AudioStream(const AudioSpec& from, const AudioSpec& to):
            AudioStream{SDL_CreateAudioStream(from.legacy_addr(), to.legacy_addr())} {

        }

        void set_format(const AudioSpec& from, const AudioSpec& to) {
            if (!SDL_SetAudioStreamFormat(handle, from.legacy_addr(), to.legacy_addr()))
                throw Error{};
        }

        float frequency() const {
            if (const auto ret = SDL_GetAudioStreamFrequencyRatio(handle)==0.0f)
                throw Error{};
            else
                return ret;
        }

        void frequency(const float v) const {
            if (!SDL_SetAudioStreamFrequencyRatio(handle, v))
                throw Error{};
        }

        float gain() const {
            if (const auto ret = SDL_GetAudioStreamGain(handle)==0.0f)
                throw Error{};
            else
                return ret;
        }

        void gain(const float v) const {
            if (!SDL_SetAudioStreamGain(handle, v))
                throw Error{};
        }

        List<int> input_channels() const {
            int size;
            auto data = SDL_GetAudioStreamInputChannelMap(handle, &size);
            if (!data)
                throw Error{};
            return {data, size};
        }

        void input_channels(const List<int>& v)  {
            if (!SDL_SetAudioStreamInputChannelMap(handle, v.data, v.size))
                throw Error{};
        }

        List<int> output_channels() const {
            int size;
            auto data = SDL_GetAudioStreamOutputChannelMap(handle, &size);
            if (!data)
                throw Error{};
            return {data, size};
        }

        // ReSharper disable once CppMemberFunctionMayBeConst
        void output_channels(const List<int>& v)  {
            if (!SDL_SetAudioStreamOutputChannelMap(handle, v.data, v.size))
                throw Error{};
        }

        // ReSharper disable CppMemberFunctionMayBeConst
        void bind(const AudioDevice& device);

        void rebind(const AudioDevice& device) {
            unbind();
            bind(device);
        }

        void unbind() noexcept {
            return SDL_UnbindAudioStream(handle);
        }

        AudioDeviceID binding() const noexcept {
            return static_cast<AudioDeviceID>(SDL_GetAudioStreamDevice(handle));
        }

        template<typename string_t=std::string>
        Properties<string_t> properties() const  {
            return SDL_GetAudioStreamProperties(handle);
        }

        std::pair<AudioSpec, AudioSpec> format() const  {
            auto ret = std::pair<AudioSpec, AudioSpec>{};
            if (!SDL_GetAudioStreamFormat(handle, ret.first.legacy_addr(), ret.second.legacy_addr()))
                throw Error{};
            return ret;
        }


        void put(const std::span<const unsigned char> data) {
            if (!SDL_PutAudioStreamData(handle, data.data(), data.size()))
                throw Error{};
        }
        int get(std::span<unsigned char> data) {
            if (const auto sz = SDL_GetAudioStreamData(handle, data.data(),data.size()) < 0)
                throw Error{};
            else
                return sz;
        }
        int avaliable() const  {
            if (const auto ret = SDL_GetAudioStreamAvailable(handle) < 0)
                throw Error{};
            else
                return ret;
        }
        int queued() const  {
            if (const auto ret = SDL_GetAudioStreamQueued(handle) < 0)
                throw Error{};
            else
                return ret;
        }
        void flush()  {
            if (!SDL_FlushAudioStream(handle))
                throw Error{};
        }

        void clear()  {
            if (!SDL_ClearAudioStream(handle))
                throw Error{};
        }

        void pause() {
            if (!SDL_PauseAudioStreamDevice(handle))
                throw Error{};
        }

        void resume() {
            if (!SDL_ResumeAudioStreamDevice(handle))
                throw Error{};
        }

        void lock() {
            if (!SDL_LockAudioStream(handle))
                throw Error{};
        }

        void unlock() {
            if (!SDL_UnlockAudioStream(handle))
                throw Error{};
        }

        using Callback = SDL_AudioStreamCallback;

        void add_callback_get(Callback callback, void* data) {
            if (!SDL_SetAudioStreamGetCallback(handle, callback, data))
                throw Error{};
        }

        void add_callback_put(Callback callback, void* data) {
            if (!SDL_SetAudioStreamPutCallback(handle, callback, data))
                throw Error{};
        }

        ~AudioStream() noexcept{
            SDL_DestroyAudioStream(handle);
        }

        template<std::ranges::range Range>
        static void unbind(const Range& streams){
            auto cache = std::vector<SDL_AudioStream*>{};
            for (AudioStream& e: streams)
                cache.emplace_back(e.handle);

            SDL_UnbindAudioStreams(cache.data(), cache.size());
        }


        template<std::ranges::range Range>
        static void bind(const AudioDevice& device, const Range& streams);
    };



    struct AudioDevice {
        AudioDeviceID id;

        // this id will NOT promise to be the member
        // return value of SDL_OpenAudioDevice will be used
        AudioDevice(AudioDeviceID id, const AudioSpec& spec):
            id{SDL_OpenAudioDevice(legacy_id(), reinterpret_cast<const SDL_AudioSpec*>(&spec))}{
            if (static_cast<Uint32>(id) == 0)
                throw Error{};
        }

        explicit AudioDevice(AudioDeviceID id):
            id{SDL_OpenAudioDevice(legacy_id(), nullptr)}{
            if (static_cast<Uint32>(id) == 0)
                throw Error{};
        }

        ~AudioDevice() noexcept {
            SDL_CloseAudioDevice(legacy_id());
        }

        AudioDevice duplicate() const {
            return AudioDevice{id};
        }

        AudioDevice duplicate(const AudioSpec& spec) const {
            return AudioDevice{id, spec};
        }


        const char* name() const {
            auto name = SDL_GetAudioDeviceName(legacy_id());
            if (!name)
                throw Error{};
            return name;
        }

        std::pair<AudioSpec, int> format() const {
            std::pair<AudioSpec, int> ret;
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

        bool paused() const  {

            return SDL_AudioDevicePaused(legacy_id());
        }

        float gain() const  {
            if (const auto ret = SDL_GetAudioDeviceGain(legacy_id()) == -1.0f)
                throw Error{};
            else
                return ret;
        }

        constexpr SDL_AudioDeviceID
            legacy_id() const noexcept {
            return static_cast<SDL_AudioDeviceID>(id);
        }

        void pause() {
            if (!SDL_PauseAudioDevice(legacy_id()))
                throw Error{};
        }

        void resume() {
            if (!SDL_ResumeAudioDevice(legacy_id()))
                throw Error{};
        }

        void set_gain(const float gain)  {
            if (!SDL_SetAudioDeviceGain(legacy_id(), gain))
                throw Error{};
        }



        template<typename... Args>
        static AudioDevice open_default_playback(Args&& arg) {
            return AudioDevice{default_playback, std::forward<Args>(arg)...};
        }

        template<typename... Args>
        static AudioDevice open_default_recording(Args&& arg) {
            return AudioDevice{default_recording, std::forward<Args>(arg)...};
        }
        [[nodiscard]]
        static List<AudioDevice>
            playback_devices()  {
            int size;
            auto data = SDL_GetAudioPlaybackDevices(&size);
            if (!data)
                throw Error{};
            return {reinterpret_cast<AudioDevice*>(data), size};
        }

        AudioStream open(const AudioSpec& spec, AudioStream::Callback callback=nullptr, void* data=nullptr) {
            if (const auto stream_id = SDL_OpenAudioDeviceStream(legacy_id(), spec.legacy_addr(), callback, data);
                !stream_id)
                throw Error{};
            else
                return AudioStream{stream_id};

        }

        using PostmixCallback = SDL_AudioPostmixCallback;

        void setAudioPostmixCallback(PostmixCallback callback, void* data) {
            if (!SDL_SetAudioPostmixCallback(legacy_id(), callback, data))
                throw Error{};
        }

        [[nodiscard]]
        static List<AudioDevice>
            recording_devices()  {
            int size;
            auto data = SDL_GetAudioRecordingDevices(&size);
            if (!data)
                throw Error{};
            return {reinterpret_cast<AudioDevice*>(data), size};
        }
    };

    inline void AudioStream::bind(const AudioDevice& device) {
        if (!SDL_BindAudioStream(device.legacy_id(), handle))
            throw Error{};
    }

    template<std::ranges::range Range>
    void AudioStream::bind(const AudioDevice& device, const Range& streams) {
        auto cache = std::vector<SDL_AudioStream*>{};
        for (AudioStream& e: streams)
            cache.emplace_back(e.handle);

        if (!SDL_BindAudioStreams(device.legacy_id(), cache.data(), cache.size()))
            throw Error{};
    }


    inline std::pair<AudioSpec, List<Uint8>> load_wav(SDL_IOStream* ios, bool close) {
        auto ret = std::pair<AudioSpec, List<Uint8>>{};

        Uint32 size;
        if (!SDL_LoadWAV_IO(ios, close, ret.first.legacy_addr(), &ret.second.data, &size))
            throw Error{};
        ret.second.size = size;
        return std::move(ret);
    }

    inline std::pair<AudioSpec, List<Uint8>> load_wav(const std::filesystem::path& path) {
        auto ret = std::pair<AudioSpec, List<Uint8>>{};
        Uint32 size;
        if (!SDL_LoadWAV(reinterpret_cast<const char*>(path.generic_u8string().c_str()), ret.first.legacy_addr(), &ret.second.data, &size))
            throw Error{};
        ret.second.size = size;
        return std::move(ret);
    }

    inline void mix_audio(Uint8 *dst, const Uint8 *src, AudioFormat format, Uint32 len, float volume) {
        if (!SDL_MixAudio(dst, src, static_cast<SDL_AudioFormat>(format), len, volume))
            throw Error{};
    }

    inline void convert_samples(const AudioSpec& src_spec, const List<Uint8>& src, const AudioSpec& dst_spec, List<Uint8>& dst) {
        if (!SDL_ConvertAudioSamples(src_spec.legacy_addr(), src.data, src.size, dst_spec.legacy_addr(), &dst.data, &dst.size))
            throw Error{};
    }
}

#endif //SDL_AUDIO_HPP
