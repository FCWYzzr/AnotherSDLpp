//
// Created by FCWY on 25-2-27.
//

#ifndef SDL_STDINC_HPP
#define SDL_STDINC_HPP
#include "SDL3/SDL_stdinc.h"
#include <concepts>
#include <memory>
#include <functional>

namespace SDL::plus {
    struct Error final: std::runtime_error{
        Error() noexcept:
            runtime_error{SDL_GetError()}{}
    };
}

namespace SDL {
    using UnderlyingType = int;
    constexpr std::size_t size_max = SIZE_MAX;

    template<typename T, size_t N>
    constexpr std::size_t arraysize(const T(&)[N]) {
        return N;
    }

    constexpr std::uint32_t fourcc(
        const std::uint8_t ll,
        const std::uint8_t l,
        const std::uint8_t h,
        const std::uint8_t hh) {
        return  static_cast<uint32_t>(hh) << 24
            |   static_cast<uint32_t>(h) << 16
            |   static_cast<uint32_t>(l) << 8
            |   static_cast<uint32_t>(ll);
    }

    constexpr Sint8 max_sint8 = SDL_MAX_SINT8;
    constexpr Sint8 min_sint8 = SDL_MIN_SINT8;
    constexpr Uint8 max_uint8 = SDL_MAX_UINT8;
    constexpr Uint8 min_uint8 = SDL_MIN_UINT8;

    constexpr Sint16 max_int16 = SDL_MAX_SINT16;
    constexpr Sint16 min_int16 = SDL_MIN_SINT16;
    constexpr Uint16 max_uint16 = SDL_MAX_UINT16;
    constexpr Uint16 min_uint16 = SDL_MIN_UINT16;

    constexpr Sint32 max_int32 = SDL_MAX_SINT32;
    constexpr Sint32 min_int32 = SDL_MIN_SINT32;
    constexpr Uint32 max_uint32 = SDL_MAX_UINT32;
    constexpr Uint32 min_uint32 = SDL_MIN_UINT32;

    constexpr Sint64 max_int64 = SDL_MAX_SINT64;
    constexpr Sint64 min_int64 = SDL_MIN_SINT64;
    constexpr Uint64 max_uint64 = SDL_MAX_UINT64;
    constexpr Uint64 min_uint64 = SDL_MIN_UINT64;

    constexpr SDL_Time max_time = SDL_MAX_TIME;
    constexpr SDL_Time min_time = SDL_MIN_TIME;


    constexpr float float_epsion = SDL_FLT_EPSILON;

    inline const char* GetError() noexcept {
        return SDL_GetError();
    }

    inline namespace memory {
        template<typename T=std::uint8_t>
        T* malloc() {
            auto v = static_cast<T*>(SDL_malloc(sizeof(T)));
            if (!v)
                throw plus::Error{};
            return v;
        }

        template<typename T=std::uint8_t>
        T* calloc(const std::size_t length) noexcept {
            auto v = static_cast<T*>(SDL_calloc(length, sizeof(T)));
            if (!v)
                throw plus::Error{};
            return v;
        }

        template<typename T=std::uint8_t>
        T* realloc(T* const mem, const std::size_t neosize) noexcept {
            auto v = static_cast<T*>(SDL_realloc(static_cast<void*>(mem), neosize));
            if (!v)
                throw plus::Error{};
            return v;
        }

        template<typename T>
        void free(T* const mem) noexcept {
            return SDL_free(static_cast<void*>(mem));
        }

        // noexcept is a part of func type since c++17
        using malloc_func = void *(SDLCALL*)(size_t size) noexcept;
        using calloc_func = void *(SDLCALL *)(size_t n_member, size_t member_size) noexcept;
        using realloc_func = void *(SDLCALL *)(void*, size_t) noexcept;
        using free_func = void (SDLCALL *)(void *) noexcept;

        struct MemoryFunctions {
            malloc_func
                malloc;
            calloc_func
                calloc;
            realloc_func
                realloc;
            free_func
                free;
        };

        inline MemoryFunctions
            GetOriginalMemoryFunctions() noexcept {
            MemoryFunctions cache;
            SDL_GetOriginalMemoryFunctions(
                reinterpret_cast<SDL_malloc_func*>(&cache.malloc),
                reinterpret_cast<SDL_calloc_func*>(&cache.calloc),
                reinterpret_cast<SDL_realloc_func*>(&cache.realloc),
                reinterpret_cast<SDL_free_func*>(&cache.free)
            );
            return cache;
        }

        inline MemoryFunctions
            GetMemoryFunctions() noexcept {
            MemoryFunctions cache;
            SDL_GetMemoryFunctions(
                reinterpret_cast<SDL_malloc_func*>(&cache.malloc),
                reinterpret_cast<SDL_calloc_func*>(&cache.calloc),
                reinterpret_cast<SDL_realloc_func*>(&cache.realloc),
                reinterpret_cast<SDL_free_func*>(&cache.free)
            );
            return cache;
        }

        // throw an exception is much more heavy than bool result
        // so users shall call GetError by themselves
        inline bool SetMemoryFunctions(
            const MemoryFunctions& function_pack
        ) noexcept {
            return SDL_SetMemoryFunctions(
                function_pack.malloc,
                function_pack.calloc,
                function_pack.realloc,
                function_pack.free
            );
        }

        // aligned mem
        namespace aligned {
            inline void* alloc(const size_t alignment, const size_t size) noexcept {
                return SDL_aligned_alloc(alignment, size);
            }

            template<typename T>
            void* free(T* const mem) noexcept {
                return SDL_aligned_free(mem);
            }
        }

        inline int GetNumAllocations() noexcept {
            return SDL_GetNumAllocations();
        }
    }

    namespace plus::inline memory {
        template<typename T, bool is_aligned>
        struct Deletor;

        template<typename T>
        struct Deletor<T, false> {
            constexpr void operator () (T* const ptr) noexcept {
                SDL::memory::free(ptr);
            }
        };

        template<typename T>
        struct Deletor<T, true> {
            constexpr void operator () (T* const ptr) noexcept {
                aligned::free(ptr);
            }
        };

        template<typename T, bool is_aligned=false>
        using unique_ptr = std::unique_ptr<T, Deletor<T, is_aligned>>;

        template<typename T, typename... Args>
        requires std::constructible_from<T, Args...> && !std::is_array_v<T>
        unique_ptr<T> make_unique(Args&&... args) noexcept(noexcept(T{args...})) {
            return {new (SDL::memory::malloc<T>()) T {std::forward<Args>(args)...}, free<>};
        }
    }

    class Environment{
        using handle_t = SDL_Environment*;

        handle_t
            handle;

    public:
        // ReSharper disable CppMemberFunctionMayBeConst
        explicit Environment(const bool populated) noexcept:
            handle{SDL_CreateEnvironment(populated)} {}

        Environment(const Environment&)=delete;
        Environment& operator=(const Environment&)=delete;

        Environment(Environment&& expired) noexcept:
            handle{nullptr} {
            std::swap(expired.handle, handle);
        }
        Environment& operator = (Environment&& expired) noexcept {
            std::swap(expired.handle, handle);
            return *this;
        }

        const char* getVariable(const std::string_view key) const noexcept {
            return SDL_GetEnvironmentVariable(handle, key.data());
        }

        plus::unique_ptr<char*> getVariables() const noexcept {
            return plus::unique_ptr<char*>{
                SDL_GetEnvironmentVariables(handle)
            };
        }

        bool setVariable(const std::string_view key, const std::string_view value, const bool overwrite=false) noexcept {
            return SDL_SetEnvironmentVariable(
                handle,
                key.data(),
                value.data(),
                overwrite
            );
        }
        bool unsetVariable(const std::string_view key) const noexcept {
            return SDL_UnsetEnvironmentVariable(
                handle,
                key.data()
            );
        }

        static Environment process_environment() noexcept {
            return {SDL_GetEnvironment()};
        }

        // ReSharper disable once CppNonExplicitConvertingConstructor
        Environment(const handle_t handle=nullptr) noexcept:
            handle{handle} {}

        // ReSharper disable once CppNonExplicitConversionOperator
        operator handle_t() const noexcept {return handle;}
        handle_t release() noexcept {
            const auto tmp = handle;
            handle = nullptr;
            return tmp;
        }
        ~Environment() noexcept {
            if (handle)
                SDL_DestroyEnvironment(handle);
        }

        // plus:

        struct VariablePack {
            using iterator = char**;
            plus::unique_ptr<char*>
                pack;

            auto begin() const noexcept {
                return pack.get();
            }

            auto end() const noexcept {
                auto it = pack.get();
                while (*it != nullptr)
                    ++ it;
                return it;
            }
        };

        struct Viewer {
            handle_t
                handle;
            std::string
                key;

            // ReSharper disable once CppNonExplicitConversionOperator
            operator const char*() const noexcept {
                return SDL_GetEnvironmentVariable(handle, key.data());
            }
        };

        struct Visiter: Viewer {
            bool operator = (const std::string_view value) const noexcept {
                return SDL_SetEnvironmentVariable(handle, key.data(), value.data(), true);
            }
        };

        Viewer operator [] (const std::string_view key) const noexcept {
            return {handle, std::string{key}};
        }
        Visiter operator [] (const std::string_view key) noexcept {
            return {handle, std::string{key}};
        }
        VariablePack dump() const noexcept {
            return {getVariables()};
        }

        bool try_insert(const std::string_view key, const std::string_view value) noexcept {
            return SDL_SetEnvironmentVariable(
                handle,
                key.data(),
                value.data(),
                false
            );
        }
        bool insert(const std::string_view key, const std::string_view value) noexcept {
            return SDL_SetEnvironmentVariable(
                handle,
                key.data(),
                value.data(),
                true
            );
        }
        // ReSharper restore CppMemberFunctionMayBeConst
    };

    inline namespace system {
        inline const char* getenv(const std::string_view key) noexcept {
            return SDL_getenv(key.data());
        }
        namespace unsafe {
            inline const char* getenv(const std::string_view key) noexcept {
                return SDL_getenv_unsafe(key.data());
            }
            inline bool setenv(const std::string_view key, const std::string_view value, const bool overwrite) noexcept {
                return SDL_setenv_unsafe(key.data(), value.data(), overwrite) != 0;
            }
            inline bool unsetenv(const std::string_view key) noexcept {
                return SDL_unsetenv_unsafe(key.data()) != 0;
            }
        }
    }

    inline namespace utilities {
        template<typename T>
        using CompareCallback = int (SDLCALL *)(const T&, const T&) noexcept;

        template<typename T>
        [[deprecated("why don't you use std::ranges::sort ?")]]
        void qsort(T* base, const size_t length, const CompareCallback<T> cmp) noexcept {
            return SDL_qsort(base, length, sizeof(T), [cmp](const void* a, const void* b) {
                return cmp(reinterpret_cast<T&>(*a), reinterpret_cast<T&>(*b));
            });
        }

        void* SDLCALL SDL_bsearch(const void *key, const void *base, size_t nmemb, size_t size, SDL_CompareCallback compare);

    }



}

namespace SDL::plus {
    inline namespace memory {
        template<typename T, bool is_aligned=false>
        struct Allocator;


        template<typename T>
        requires std::same_as<std::remove_cvref_t<T>, T>
        struct Allocator<T, false> {
            using value_type = T;
            using is_always_equal = std::true_type;

            constexpr Allocator() noexcept=default;

            static T* allocate(std::size_t n) noexcept{
                return SDL::malloc<T>(n);
            }
            static void deallocate(T* p, std::size_t) noexcept {
                return SDL::free(p);
            }
        };

        template<typename T>
        requires std::same_as<std::remove_cvref_t<T>, T>
        struct Allocator<T, true> {
            using value_type = T;
            // SDL_aligned_free can free any alignment ptr
            // so any aligned allocator is equivalent
            using is_always_equal = std::true_type;

            std::size_t alignment;
            constexpr explicit Allocator(const std::size_t alignment) noexcept:
                alignment{alignment} {}

            void* allocate(const std::size_t n) const noexcept{
                return aligned::alloc(alignment, n);
            }
            static void deallocate(T* p, std::size_t) noexcept {
                return aligned::free(p);
            }
        };
    }
}


#endif //SDL_STDINC_HPP
