//
// Created by FCWY on 25-3-17.
//

#ifndef SDL_IOSTREAM_HPP
#define SDL_IOSTREAM_HPP
#include <SDL3/SDL_iostream.h>
#include "SDL_stdinc.hpp"
#include "SDL_error.hpp"
#include "SDL_properties.hpp"
#include <cstddef>

namespace SDL::inline iostream {

    struct Closer {
        void operator () (SDL_IOStream* p) const noexcept {
            SDL_CloseIO(p);
        }
    };

    using IOStream = std::unique_ptr<SDL_IOStream, Closer>;

    constexpr auto ios_size = [](void* ptr) noexcept {
            try {
                auto& ios = *static_cast<std::istream*>(ptr);
                ios . seekg(0, std::ios_base::end);
                auto cnt = ios . tellg();
                ios . seekg(0, std::ios_base::beg);
                return static_cast<Sint64>(cnt);
            }
            catch (std::exception&) {
                return -1;
            }
        };
    constexpr auto i_seek = [](void* ptr, Sint64 offset, SDL_IOWhence whence) noexcept {
        try {
            auto& ios = *static_cast<std::istream*>(ptr);
            switch (whence) {
            case SDL_IO_SEEK_SET: ios . seekg(offset, std::ios_base::beg);
                break;
            case SDL_IO_SEEK_CUR: ios . seekg(offset, std::ios_base::cur);
                break;
            case SDL_IO_SEEK_END: ios . seekg(offset, std::ios_base::end);
                break;
            }
            return static_cast<Sint64>(ios . tellg());
        }
        catch (std::exception&) {
            return -1;
        }
    };
    constexpr auto o_seek = [](void* ptr, Sint64 offset, SDL_IOWhence whence) noexcept {
        try {
            auto& ios = *static_cast<std::ostream*>(ptr);
            switch (whence) {
            case SDL_IO_SEEK_SET: ios . seekp(offset, std::ios_base::beg);
                break;
            case SDL_IO_SEEK_CUR: ios . seekp(offset, std::ios_base::cur);
                break;
            case SDL_IO_SEEK_END: ios . seekp(offset, std::ios_base::end);
                break;
            }
            return static_cast<Sint64>(ios . tellp());
        }
        catch (std::exception&) {
            return -1;
        }
    };
    constexpr auto read = [](void* ptr, void *data, size_t size, SDL_IOStatus *status) {
        auto& ios = *static_cast<std::ostream*>(ptr);
        try {
            ios.read(static_cast<char*>(data), size);
            if (ios.eof())
                *status = SDL_IO_STATUS_EOF;
            else
                *status = SDL_IO_STATUS_READY;
        }catch (std::exception&) {
            *status = SDL_IO_STATUS_ERROR;
        }
        return static_cast<size_t>(ios.tellp());
    };
    constexpr auto not_read = [](void* ptr, void *data, size_t size, SDL_IOStatus *status) {
        *status = SDL_IO_STATUS_ERROR;
        return 0ull;
    };
    constexpr auto write = [](void* ptr, const void *data, size_t size, SDL_IOStatus *status) {
        auto& ios = *static_cast<std::iostream*>(ptr);
        try {
            ios.write(static_cast<const char*>(data), size);
            if (ios.eof())
                *status = SDL_IO_STATUS_EOF;
            else
                *status = SDL_IO_STATUS_READY;
        }catch (std::exception&) {
            *status = SDL_IO_STATUS_ERROR;
        }
        return static_cast<size_t>(ios.gcount());
    };
    constexpr auto not_write = [](void* ptr, const void *data, size_t size, SDL_IOStatus *status) {
        *status = SDL_IO_STATUS_ERROR;
        return 0ull;
    };
    constexpr auto flush = [](void* ptr, SDL_IOStatus* status) {
        try {
            auto& ios = *static_cast<std::ostream*>(ptr);
            ios.flush();
            *status = SDL_IO_STATUS_READY;
            return true;
        }catch (std::exception&) {
            *status = SDL_IO_STATUS_ERROR;
            return false;
        }
    };
    constexpr auto not_flush = [](void* ptr, SDL_IOStatus* status) {
        return false;
    };
    constexpr auto close = [](void* ptr) {
        try {
            delete static_cast<std::ios*>(ptr);
            return true;
        }catch (std::exception&) {
            return false;
        }
    };

    template <std::derived_from<std::ios_base> T, typename... Args>
    IOStream open_from(Args&& args...) {
        auto family = SDL_IOStreamInterface{
            0,
            ios_size,
            i_seek,
            read,
            write,
            flush,
            close
        };
        auto delegated = std::make_unique<T>(std::forward<Args>(args)...);
        if constexpr (std::derived_from<std::iostream, T>) {
            return SDL_OpenIO(&family, delegated.release());
        }
        else if constexpr (std::derived_from<std::istream, T>) {
            family.write = not_write;
            family.flush = not_flush;
            return SDL_OpenIO(&family, delegated.release());
        }
        else if constexpr (std::derived_from<std::ostream, T>) {
            family.seek = o_seek;
            family.read = not_read;
            return SDL_OpenIO(&family, delegated.release());
        }
        else
            return nullptr;
    }

}

#endif //SDL_IOSTREAM_HPP
