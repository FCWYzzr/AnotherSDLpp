//
// Created by FCWY on 25-2-27.
//

#ifndef SDL_STDINC_HPP
#define SDL_STDINC_HPP
#include "SDL3/SDL_stdinc.h"
#include <concepts>
#include <memory>
#include <algorithm>
#include <ranges>
#include <functional>

namespace SDL::plus {
    struct Error final: std::runtime_error{
        Error() noexcept:
            runtime_error{SDL_GetError()}{}
    };
}

namespace SDL {
    inline namespace type_requirement {
        using UnderlyingType = int;
        using FunctionPointer = SDL_FunctionPointer;
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

    inline namespace limit {

        constexpr std::size_t size_max = SIZE_MAX;

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
    }


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

        template<typename T=std::uint8_t>
        T* memcpy(T* dest, const T* src, const size_t count=1) noexcept {
            // SDL_memcpy might be ::memcpy
            return ::SDL_memcpy(
                static_cast<void*>(dest),
                static_cast<const void*>(src),
                count * sizeof(T)
            );
        }

        template<typename T=std::uint8_t>
        T* memmove(T* dest, const T* src, const size_t count=1) noexcept {
            // SDL_memmove might be ::memmove
            return ::SDL_memmove(
                static_cast<void*>(dest),
                static_cast<const void*>(src),
                count * sizeof(T)
            );
        }

        template<typename T=std::uint8_t>
        [[deprecated("its not a good idea to operate raw data directly in C++, consider use constructor / std::fill_n instead")]]
        T* memset(T* dest, const int byte, const std::size_t count=1) noexcept {
            return ::SDL_memset(
                static_cast<void*>(dest),
                byte,
                count * sizeof(T)
            );
        }

        template<typename T=std::uint8_t>
        [[deprecated("its not a good idea to operate raw data directly in C++, consider use constructor / std::fill_n instead")]]
        T* memset4(T* dest, const Uint32 dword, const std::size_t count=1) noexcept {
            return SDL_memset4(
                static_cast<void*>(dest),
                dword,
                count * sizeof(T)
            );
        }

        template<typename T>
        [[deprecated("its not a good idea to operate raw data directly in C++, consider use constructor / std::fill_n instead")]]
        T* zero(T& obj) noexcept {
            return memset(&obj, 0);
        }

        template<typename T>
        [[deprecated("its not a good idea to operate raw data directly in C++, consider use constructor / std::fill_n instead")]]
        T* zero(T* ptr) noexcept {
            return memset(ptr, 0);
        }

        template<typename T, size_t N>
        [[deprecated("its not a good idea to operate raw data directly in C++, consider use constructor / std::fill_n / std::ranges::fill instead")]]
        T* zero(T (&array)[N]) noexcept {
            return memset(&array, 0);
        }

        template<typename T, typename U>
        int memcmp(const T* left, const U* right, const std::size_t size_in_byte) noexcept {
            return SDL_memcmp(
                static_cast<const void*>(left),
                static_cast<const void*>(right),
                size_in_byte
            );
        }

        struct MemoryFunctions {
            SDL_malloc_func
                malloc;
            SDL_calloc_func
                calloc;
            SDL_realloc_func
                realloc;
            SDL_free_func
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

        inline void SetMemoryFunctions(
            const MemoryFunctions& function_pack
        ) {
            if (!SDL_SetMemoryFunctions(
                function_pack.malloc,
                function_pack.calloc,
                function_pack.realloc,
                function_pack.free
            ))
                throw plus::Error{};
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

    inline namespace system {
        inline const char* getenv(const char* key) noexcept {
            return SDL_getenv(key);
        }
        namespace unsafe {
            inline const char* getenv(const char* key) noexcept {
                return SDL_getenv_unsafe(key);
            }
            inline void setenv(const char* key, const char* value, const bool overwrite) {
                if (SDL_setenv_unsafe(key, value, overwrite)== -1)
                    throw plus::Error{};
            }
            inline void unsetenv(const char* key) {
                if (SDL_unsetenv_unsafe(key) == -1)
                    throw plus::Error{};
            }
        }
    }

    inline namespace algorithm {
        using CompareCallback = int (SDLCALL *)(const void*, const void*) noexcept;

        template<typename T>
        [[deprecated("why don't you use std::ranges::sort or std::sort ?")]]
        void qsort(T* base, const size_t length, const CompareCallback cmp) noexcept {
            return SDL_qsort(
                static_cast<void*>(base),
                length,
                sizeof(T),
                cmp
            );
        }

        template<typename T>
        [[deprecated("why don't you use std::ranges::binary_search or std::binary_search ?")]]
        T* bsearch(const T& key, const T base[], const size_t length, const CompareCallback compare) noexcept {
            return SDL_bsearch(
                static_cast<const void*>(&key),
                static_cast<const void*>(base),
                length,
                sizeof(T),
                compare
            );
        }


        using CompareCallback_r = int (SDLCALL *)(void *userdata, const void *a, const void *b);


        template<typename T, typename U>
        [[deprecated("why don't you use std::ranges::sort or std::sort ?")]]
        void qsort(T* base, const size_t length, const CompareCallback_r cmp, U* userdata) noexcept {
            return SDL_qsort_r(
                static_cast<void*>(base),
                length,
                sizeof(T),
                cmp,
                static_cast<void*>(userdata)
            );
        }

        template<typename T, typename U>
        [[deprecated("why don't you use std::ranges::binary_search or std::binary_search ?")]]
        T* bsearch(const T& key, const T base[], const size_t length, const CompareCallback_r compare, U* userdata) noexcept {
            return SDL_bsearch_r(
                static_cast<const void*>(&key),
                static_cast<const void*>(base),
                length,
                sizeof(T),
                compare,
                static_cast<void*>(userdata)
            );
        }
    }

    inline namespace utilities {

        template<typename T, size_t N>
        constexpr std::size_t arraysize(const T(&)[N]) {
            return N;
        }

        inline int abs(const int i) noexcept {
            return SDL_abs(i);
        }

        template<typename T>
        T min(const T& x, const T& y) noexcept {
            return x > y ? y : x;
        }

        template<typename T>
        T max(const T& x, const T& y) noexcept {
            return x > y ? x : y;
        }

        template<typename T>
        T clamp(T&& x, const T& low, const T& high) noexcept {
            if (x < low)
                return low;
            return x < high ? x : high;
        }
    }

    inline namespace char_types{
        [[deprecated("why dont you use std::isalpha?")]]
        inline bool isalpha(const int ch) noexcept {
            return SDL_isalpha(ch);
        }

        [[deprecated("why dont you use std::isalnum?")]]
        inline bool isalnum(const int ch) noexcept {
            return SDL_isalnum(ch);
        }

        [[deprecated("why dont you use std::isblank?")]]
        inline bool isblank(const int ch) noexcept {
            return SDL_isblank(ch);
        }

        [[deprecated("why dont you use std::iscntrl?")]]
        inline bool iscntrl(const int ch) noexcept {
            return SDL_iscntrl(ch);
        }

        [[deprecated("why dont you use std::isdigit?")]]
        inline bool isdigit(const int ch) noexcept {
            return SDL_isdigit(ch);
        }

        [[deprecated("why dont you use std::isxdigit?")]]
        inline bool isxdigit(const int ch) noexcept {
            return SDL_isxdigit(ch);
        }

        [[deprecated("why dont you use std::ispunct")]]
        inline bool ispunct(const int ch) noexcept {
            return SDL_ispunct(ch);
        }

        [[deprecated("why dont you use std::isspace?")]]
        inline bool isspace(const int ch) noexcept {
            return SDL_isspace(ch);
        }

        [[deprecated("why dont you use std::isupper?")]]
        inline bool isupper(const int ch) noexcept {
            return SDL_isupper(ch);
        }

        [[deprecated("why dont you use std::islower?")]]
        inline bool islower(const int ch) noexcept {
            return SDL_islower(ch);
        }

        [[deprecated("why dont you use std::isprint?")]]
        inline bool isprint(const int ch) noexcept {
            return SDL_isprint(ch);
        }

        [[deprecated("why dont you use std::isgraph?")]]
        inline bool isgraph(const int ch) noexcept {
            return SDL_isgraph(ch);
        }


        [[deprecated("why dont you use std::toupper?")]]
        inline int toupper(const int ch) noexcept {
            return SDL_toupper(ch);
        }

        [[deprecated("why dont you use std::tolower?")]]
        inline int tolower(const int ch) noexcept {
            return SDL_tolower(ch);
        }
    }

    inline namespace hash {
        inline Uint16 crc16(const Uint16 crc, const void *data, const size_t len) noexcept {
            return SDL_crc16(crc, data, len);
        }

        inline Uint32 crc32(const Uint32 crc, const void *data, const size_t len) noexcept {
            return SDL_crc32(crc, data, len);
        }
    }

    inline namespace wstring {
        inline std::size_t wcslen(const wchar_t *s) noexcept {
            return SDL_wcslen(s);
        }

        inline std::size_t wcsnlen(const wchar_t* s, const std::size_t max_size) noexcept {
            return SDL_wcsnlen(s, max_size);
        }

        inline std::size_t wcslcpy(wchar_t* dest, const wchar_t* src, const std::size_t max_size) noexcept {
            return SDL_wcslcpy(dest, src, max_size);
        }

        inline std::size_t wcslcat(wchar_t* dest, const wchar_t* src, const std::size_t max_size) noexcept {
            return SDL_wcslcat(dest, src, max_size);
        }

        inline plus::unique_ptr<wchar_t> wcsdup(const wchar_t *wstr) noexcept {
            return plus::unique_ptr<wchar_t>{
                SDL_wcsdup(wstr)
            };
        }

        inline wchar_t* wcsstr(const wchar_t *haystack, const wchar_t *needle) noexcept {
            return SDL_wcsstr(
                haystack,
                needle
            );
        }

        inline wchar_t* wcsnstr(const wchar_t *haystack, const wchar_t *needle, const size_t maxlen) noexcept {
            return SDL_wcsnstr(
                haystack,
                needle,
                maxlen
            );
        }

        inline int wcscmp(const wchar_t *str1, const wchar_t *str2) noexcept {
            return SDL_wcscmp(str1, str2);
        }

        inline int wcsncmp(const wchar_t *str1, const wchar_t *str2, const std::size_t maxlen) noexcept {
            return SDL_wcsncmp(str1, str2, maxlen);
        }

        inline int wcscasecmp(const wchar_t *str1, const wchar_t *str2) noexcept {
            return SDL_wcscasecmp(str1, str2);
        }

        inline int wcsncasecmp(const wchar_t *str1, const wchar_t *str2, const std::size_t maxlen) noexcept {
            return SDL_wcsncasecmp(str1, str2, maxlen);
        }

        inline long wcstol(const wchar_t *str, wchar_t **endp, const int base) noexcept {
            return SDL_wcstol(str, endp, base);
        }
    }

    inline namespace string {
        inline std::size_t strlen(const char *s) noexcept {
            return SDL_strlen(s);
        }

        inline std::size_t strnlen(const char* s, const std::size_t max_size) noexcept {
            return SDL_strnlen(s, max_size);
        }

        inline std::size_t strlcpy(char* dest, const char* src, const std::size_t max_size) noexcept {
            return SDL_strlcpy(dest, src, max_size);
        }

        inline std::size_t utf8strlcpy(char *dst, const char *src, const std::size_t dst_bytes) noexcept {
            return SDL_utf8strlcpy(dst, src, dst_bytes);
        }

        inline std::size_t strlcat(char* dest, const char* src, const std::size_t max_size) noexcept {
            return SDL_strlcat(dest, src, max_size);
        }

        inline plus::unique_ptr<char> strdup(const char *str) noexcept {
            return plus::unique_ptr<char>{
                SDL_strdup(str)
            };
        }

        inline plus::unique_ptr<char> strndup(const char *str, const std::size_t maxlen) noexcept {
            return plus::unique_ptr<char>{
                SDL_strndup(str, maxlen)
            };
        }

        inline char* strrev(char* str) noexcept {
            return SDL_strrev(str);
        }

        inline char* strupr(char* str) noexcept {
            return SDL_strupr(str);
        }

        inline char* strlwr(char* str) noexcept {
            return SDL_strlwr(str);
        }

        inline char* strchr(const char* str, const int c) noexcept {
            return SDL_strchr(str, c);
        }

        inline char* strrchr(const char* str, const int c) noexcept {
            return SDL_strrchr(str, c);
        }

        inline char* strstr(const char *haystack, const char *needle) noexcept {
            return SDL_strstr(
                haystack,
                needle
            );
        }

        inline char* strnstr(const char *haystack, const char *needle, const size_t maxlen) noexcept {
            return SDL_strnstr(
                haystack,
                needle,
                maxlen
            );
        }

        inline char* strcasestr(const char *haystack, const char *needle) noexcept {
            return SDL_strcasestr(haystack, needle);
        }

        inline char* strtok_r(char *str, const char *delim, char **saveptr) noexcept {
            return SDL_strtok_r(str, delim, saveptr);
        }

        inline std::size_t utf8strlen(const char* str) noexcept {
            return SDL_utf8strlen(str);
        }

        inline std::size_t utf8strnlen(const char* str, const std::size_t maxlen) noexcept {
            return SDL_utf8strnlen(str, maxlen);
        }

        inline char* itoa(const int value, char *str, const int radix) noexcept {
            return SDL_itoa(value, str, radix);
        }

        inline char* uitoa(const unsigned int value, char *str, const int radix) noexcept {
            return SDL_uitoa(value, str, radix);
        }

        inline char* ltoa(const long value, char *str, const int radix) noexcept {
            return SDL_ltoa(value, str, radix);
        }

        inline char* ultoa(const unsigned long value, char *str, const int radix) noexcept {
            return SDL_ultoa(value, str, radix);
        }

        inline char* lltoa(const long long value, char *str, const int radix) noexcept {
            return SDL_lltoa(value, str, radix);
        }

        inline char* ulltoa(const unsigned long long value, char *str, const int radix) noexcept {
            return SDL_ulltoa(value, str, radix);
        }

        inline int atoi(const char *str) noexcept {
            return SDL_atoi(str);
        }

        inline double atof(const char *str) noexcept {
            return SDL_atof(str);
        }

        inline long strtol(const char *str, char **endp, const int base) noexcept {
            return SDL_strtol(str, endp, base);
        }

        inline unsigned long strtoul(const char *str, char **endp, const int base) noexcept {
            return SDL_strtoul(str, endp, base);
        }

        inline long long strtoll(const char *str, char **endp, const int base) noexcept {
            return SDL_strtoll(str, endp, base);
        }

        inline unsigned long long strtoull(const char *str, char **endp, const int base) noexcept {
            return SDL_strtoull(str, endp, base);
        }

        inline double strtod(const char *str, char **endp) noexcept {
            return SDL_strtod(str, endp);
        }



        inline int strcmp(const char *str1, const char *str2) noexcept {
            return SDL_strcmp(str1, str2);
        }

        inline int strncmp(const char *str1, const char *str2, const std::size_t maxlen) noexcept {
            return SDL_strncmp(str1, str2, maxlen);
        }

        inline int strcasecmp(const char *str1, const char *str2) noexcept {
            return SDL_strcasecmp(str1, str2);
        }

        inline int strncasecmp(const char *str1, const char *str2, const std::size_t maxlen) noexcept {
            return SDL_strncasecmp(str1, str2, maxlen);
        }

        inline char* strpbrk(const char *str, const char *breakset) noexcept {
            return SDL_strpbrk(str, breakset);
        }
    }

    inline namespace unicode {
        constexpr int invalid_unicode_codepoint = 0xFFFD;

        inline Uint32 StepUTF8(const char** str_ref, size_t* len_ref) noexcept {
            return SDL_StepUTF8(str_ref, len_ref);
        }

        inline Uint32 StepBackUTF8(const char* beg_ref, const char** cur_ref) noexcept {
            return SDL_StepBackUTF8(beg_ref, cur_ref);
        }

        inline char* UCS4ToUTF8(const Uint32 codepoint, char *dst) noexcept {
            return SDL_UCS4ToUTF8(codepoint, dst);
        }
    }

    inline namespace sio {
        template<typename ...Ts>
        int sscanf(const char* txt, const char* fmt, Ts... args) noexcept {
            return SDL_sscanf(txt, fmt, std::forward<Ts>(args)...);
        }
        inline int vsscanf(const char* txt, const char* fmt, const va_list ap) noexcept {
            return SDL_vsscanf(txt, fmt, ap);
        }

        template<typename ...Ts>
        int snprintf(char* text, const size_t maxlen, const char* fmt, Ts... args) noexcept {
            return SDL_snprintf(text, maxlen, fmt, std::forward<Ts>(args)...);
        }
        template<typename ...Ts>
        int swprintf(wchar_t* text, const size_t maxlen, const wchar_t* fmt, Ts... args) noexcept {
            return SDL_swprintf(text, maxlen, fmt, std::forward<Ts>(args)...);
        }

        inline int vsnprintf(char* text, const size_t maxlen, const char* fmt, const va_list ap) noexcept {
            return SDL_vsnprintf(text, maxlen, fmt, ap);
        }

        inline int vswprintf(wchar_t* text, const size_t maxlen, const wchar_t* fmt, const va_list ap) noexcept {
            return SDL_vswprintf(text, maxlen, fmt, ap);
        }

        template<typename ...Ts>
        int asprintf(plus::memory::unique_ptr<char>& container, const char* fmt, Ts... args) noexcept {
            char* ptr;
            const auto ret = SDL_asprintf(&ptr, fmt, std::forward<Ts>(args)...);
            container.reset(ptr);
            return ret;
        }

        inline int vasprintf(plus::memory::unique_ptr<char>& container, const char* fmt, const va_list ap) noexcept {
            char* ptr;
            const auto ret = SDL_vasprintf(&ptr, fmt, ap);
            container.reset(ptr);
            return ret;
        }
    }

    inline namespace random {
        inline void srand(const Uint64 seed) noexcept {
            SDL_srand(seed);
        }

        inline Sint32 rand(const Sint32 n) noexcept {
            return SDL_rand(n);
        }

        inline float randf() noexcept{
            return SDL_randf();
        }

        inline Uint32 rand_bits() noexcept{
            return SDL_rand_bits();
        }

        inline Sint32 rand_r(Uint64 *state, const Sint32 n) noexcept{
            return SDL_rand_r(state, n);
        }

        inline float randf_r(Uint64 *state) noexcept{
            return SDL_randf_r(state);
        }

        inline Uint32 rand_bits_r(Uint64 *state) noexcept{
            return SDL_rand_bits_r(state);
        }
    }

    inline namespace math {
        template<std::floating_point F=double>
        constexpr F pi = 3.141592653589793238462643383279502884;
        template<>
        constexpr float pi<float> = 3.141592653589793238462643383279502884F;

        template<typename F>
        F acos(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_acosf(x);
            else
                return SDL_acos(x);
        }

        template<typename F>
        F cos(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_cosf(x);
            else
                return SDL_cos(x);
        }

        template<typename F>
        F asin(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_asinf(x);
            else
                return SDL_asin(x);
        }

        template<typename F>
        F sin(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_sinf(x);
            else
                return SDL_sin(x);
        }

        template<typename F>
        F atan(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_atanf(x);
            else
                return SDL_atan(x);
        }

        template<typename F>
        F tan(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_tanf(x);
            else
                return SDL_tan(x);
        }

        template<typename F>
        F ceil(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_ceilf(x);
            else
                return SDL_ceil(x);
        }

        template<typename F>
        F exp(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_expf(x);
            else
                return SDL_exp(x);
        }

        template<typename F>
        F fabs(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_fabsf(x);
            else
                return SDL_fabs(x);
        }

        template<typename F>
        F sqrt(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_sqrtf(x);
            else
                return SDL_sqrt(x);
        }

        template<typename F>
        F floor(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_floorf(x);
            else
                return SDL_floor(x);
        }

        template<typename F>
        F round(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_roundf(x);
            else
                return SDL_round(x);
        }

        template<typename F>
        F trunc(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_truncf(x);
            else
                return SDL_trunc(x);
        }

        template<typename F>
        F log(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_logf(x);
            else
                return SDL_log(x);
        }

        template<typename F>
        F log10(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_log10f(x);
            else
                return SDL_log10(x);
        }


        template<std::floating_point F>
        bool isinf(F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_isinff(x);
            else
                return SDL_isinf(x);
        }

        template<std::floating_point F>
        bool isnan(F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_isnanf(x);
            else
                return SDL_isnan(x);
        }

        template<typename F>
        long lround(const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_lroundf(x);
            else
                return SDL_lround(x);
        }



        template<typename F>
        F copysign(const F x, const F y) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_copysignf(x, y);
            else
                return SDL_copysign(x, y);
        }

        template<typename F>
        F atan2(const F y, const F x) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_atan2f(y, x);
            else
                return SDL_atan2(y, x);
        }


        template<typename F>
        F fmod(const F x, const F y) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_fmodf(x, y);
            else
                return SDL_fmod(x, y);
        }

        template<typename F>
        F pow(const F x, const F y) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_powf(x, y);
            else
                return SDL_pow(x, y);
        }

        template<typename F>
        F modf(const F x, F* y) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_modff(x, y);
            else
                return SDL_modf(x, y);
        }

        template<typename F>
        F scalebn(const F x, const int n) noexcept {
            if constexpr (std::same_as<F, float>)
                return SDL_scalebnf(x, n);
            else
                return SDL_scalebn(x, n);
        }

    }

    namespace overflow_checking {
        inline bool mul(const std::size_t a, const std::size_t b, std::size_t& ret) noexcept {
            return SDL_size_mul_check_overflow(a, b, &ret);
        }

        inline bool add(const std::size_t a, const std::size_t b, std::size_t& ret) noexcept {
            return SDL_size_add_check_overflow(a, b, &ret);
        }
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

    inline namespace env {
        struct Environment{
            using handle_t = SDL_Environment*;

            handle_t
                handle{nullptr};

            // ReSharper disable CppMemberFunctionMayBeConst
            explicit Environment(const bool populated) noexcept:
                handle{SDL_CreateEnvironment(populated)} {
                // ReSharper disable once CppDFAConstantConditions
                if (handle == nullptr)
                    // ReSharper disable once CppDFAUnreachableCode
                    throw Error{};
            }
            Environment() noexcept=default;
            Environment(const Environment&)=delete;
            void operator=(const Environment&)=delete;

            Environment(Environment&& expired) noexcept {
                std::swap(expired.handle, handle);
            }
            Environment& operator = (Environment&& expired) noexcept {
                std::swap(expired.handle, handle);
                return *this;
            }

            const char* getVariable(const char* key) const noexcept {
                return SDL_GetEnvironmentVariable(handle, key);
            }

            plus::unique_ptr<char*> getVariables() const noexcept {
                return plus::unique_ptr<char*>{
                    SDL_GetEnvironmentVariables(handle)
                };
            }

            bool setVariable(const char* key, const char* value, const bool overwrite=false) noexcept {
                return SDL_SetEnvironmentVariable(
                    handle,
                    key,
                    value,
                    overwrite
                );
            }
            bool unsetVariable(const char* key) const noexcept {
                return SDL_UnsetEnvironmentVariable(
                    handle,
                    key
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
                bool operator = (const char* value) const noexcept {
                    return SDL_SetEnvironmentVariable(handle, key.data(), value, true);
                }
            };

            Viewer operator [] (const char* key) const noexcept {
                return {handle, std::string{key}};
            }
            Visiter operator [] (const char* key) noexcept {
                return {handle, std::string{key}};
            }
            VariablePack dump() const noexcept {
                return {getVariables()};
            }

            bool try_insert(const char* key, const char* value) noexcept {
                return SDL_SetEnvironmentVariable(
                    handle,
                    key,
                    value,
                    false
                );
            }
            bool insert(const char* key, const char* value) noexcept {
                return SDL_SetEnvironmentVariable(
                    handle,
                    key,
                    value,
                    true
                );
            }
            // ReSharper restore CppMemberFunctionMayBeConst
        };
    }

    inline namespace hash {
        template<typename Range>
        Uint16 crc16(Range&& rng, Uint16 init=0) noexcept {
            auto beg = std::ranges::cbegin(rng);
            auto end = std::ranges::cend(rng);
            while (beg != end) {
                init = SDL::crc16(init, static_cast<const void*>(&*beg), sizeof(*beg));
                ++ beg;
            }
            return init;
        }

        template<typename Range>
        Uint32 crc32(Range&& rng, Uint32 init=0) noexcept {
            auto beg = std::ranges::cbegin(rng);
            const auto& end = std::ranges::cend(rng);
            while (beg != end) {
                init = SDL::crc32(init, static_cast<const void*>(&*beg), sizeof(*beg));
                ++ beg;
            }
            return init;
        }
    }

    inline namespace unicode {
        struct U8String {
            const char* beg;
            size_t len;

            explicit U8String(const std::string_view sv) noexcept:
                beg{sv.data()},
                len{sv.length()} {}

            U8String(const char* const beg, const size_t len) noexcept:
                beg{beg},
                len{len} {}

            struct iterator {
                const char* const   buf;
                const size_t        size;
                const char* cur{buf};
                size_t      rem{size};

                Uint32 ch{0};

                /**
                 * returns: ch returns by last operation.
                 * NOT current char after calling operator --
                 */
                Uint32 operator *() const noexcept {
                    return ch;
                }

                iterator& operator ++ () noexcept {
                    ch = StepUTF8(&cur, &rem);
                    return *this;
                }

                iterator& operator -- () noexcept {
                    ch = StepBackUTF8(buf, &cur);
                    rem = size - (cur - buf);
                    return *this;
                }

                bool operator == (const iterator& other) const noexcept {
                    // this is end()
                    if (buf == nullptr) [[unlikely]]
                        return other.buf == nullptr
                                ? true
                                : other == *this;

                    if (other.buf == nullptr) [[unlikely]]
                        return rem == 0;
                    return false;
                }
            };

            iterator begin() const noexcept {
                return iterator{beg, len};
            }
            static iterator end() noexcept {
                return iterator{nullptr, 0};
            }
        };
    }

    inline namespace math {
        template<std::floating_point F>
        struct Number{
            F integer_part;
            F floating_part;
        };

        template<std::floating_point F>
        Number<F> modf(const F x) noexcept {
            auto blk = Number<F>{};
            blk.floating_part = SDL::modf(x, &blk.integer_part);
            return blk;
        }
    }

    inline namespace codecvt {
        struct IConv {
            enum class Err: std::size_t {
                GENERIC = static_cast<std::size_t>(-1),
                TOO_BIG = static_cast<std::size_t>(-2),
                ILSEQ = static_cast<std::size_t>(-3),
                INVAL = static_cast<std::size_t>(-4),
            };
            using enum Err;
            using handle_t = SDL_iconv_t;

            handle_t
                handle{nullptr};

            IConv() noexcept=default;
            IConv(const char* from, const char* to) noexcept:
                handle{SDL_iconv_open(from, to)} {
                // ReSharper disable once CppDFAConstantConditions
                if (handle == nullptr)
                    // ReSharper disable once CppDFAUnreachableCode
                    throw Error{};
            }

            IConv(const IConv&)=delete;
            void operator = (const IConv&)=delete;

            IConv(IConv&& expired) noexcept {
                std::swap(expired.handle, handle);
            }
            IConv& operator = (IConv&& expired) noexcept {
                std::swap(expired.handle, handle);
                return *this;
            }


            ~IConv() noexcept {
                if (handle)
                    SDL_iconv_close(handle);
            }


            size_t operator () (
                const char*&    inbuf,
                size_t&         inbytesleft,
                char*&          outbuf,
                size_t&         outbytesleft) const noexcept {
                return SDL_iconv(handle, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
            }

            static unique_ptr<char> conv(
                const char *tocode,
                const char *fromcode,
                const char *inbuf,
                const size_t inbytesleft ) noexcept {
                return unique_ptr<char>{SDL_iconv_string(
                    tocode,
                    fromcode,
                    inbuf,
                    inbytesleft
                )};
            }

            static unique_ptr<char> utf8_locale(
                const char* u8str
            ) noexcept {
                return conv("", "UTF-8", u8str, strlen(u8str) + 1);
            }

            static unique_ptr<Uint16> utf8_ucs2(
                const char* u8str
            ) noexcept {
                return unique_ptr<Uint16>{reinterpret_cast<Uint16*>(conv("UCS-2", "UTF-8", u8str, strlen(u8str) + 1).release())};
            }

            static unique_ptr<Uint32> utf8_ucs4(
                const char* u8str
            ) noexcept {
                return unique_ptr<Uint32>{reinterpret_cast<Uint32*>(conv("UCS-4", "UTF-8", u8str, strlen(u8str) + 1).release())};
            }

            static unique_ptr<char> wchar_utf8(
                const wchar_t* wstr
            ) noexcept {
                return conv("UCS-4", "UTF-8", reinterpret_cast<const char*>(wstr), (wcslen(wstr) + 1) * sizeof(wchar_t));
            }
        };

        inline bool operator == (const std::size_t sz, const IConv::Err err) noexcept {
            return static_cast<std::size_t>(err) == sz;
        }
    }
}

namespace SDL::plus {
    template<typename T>
    struct List {
        T* data{nullptr};
        int size{0};

        constexpr List(T* const data, const int size) noexcept:
            data{data},
            size{size} {}

        List(const List& other)=delete;
        List& operator=(const List& other)=delete;


        List(List&& other) noexcept {
            std::swap(data, other.data);
            std::swap(size, other.size);
        }

        List& operator = (List&& other) noexcept{
            std::swap(data, other.data);
            std::swap(size, other.size);
            return *this;
        }

        ~List() {
            if (data)
                free(data);
        }

        constexpr T& operator [] (const int i) noexcept {
            return data[i];
        }
    };
}


#endif //SDL_STDINC_HPP
