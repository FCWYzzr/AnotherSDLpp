//
// Created by FCWY on 25-3-17.
//

// ReSharper disable CppMemberFunctionMayBeConst
#ifndef SDL_PROPERTIES_HPP
#define SDL_PROPERTIES_HPP
#include <SDL3/SDL_properties.h>
#include "SDL_error.hpp"
#include "SDL_stdinc.hpp"

namespace SDL::inline property {

    using PropertyCleaner = SDL_CleanupPropertyCallback;
    using PropertyVisitor = SDL_EnumeratePropertiesCallback;

    template<typename string_t=std::string>
    struct Properties {
        struct Property {
            enum class Type {
                INVALID = SDL_PROPERTY_TYPE_INVALID,
                POINTER = SDL_PROPERTY_TYPE_POINTER,
                STRING = SDL_PROPERTY_TYPE_STRING,
                NUMBER = SDL_PROPERTY_TYPE_NUMBER,
                FLOAT = SDL_PROPERTY_TYPE_FLOAT,
                BOOLEAN = SDL_PROPERTY_TYPE_BOOLEAN,
            };

            const SDL_PropertiesID id;
            const string_t name;

            template<typename T>
            requires std::is_class_v<T>
                && std::move_constructible<T>
            Property& operator = (T&& value) {
                if (auto ptr = new T{std::move(value)};
                    !SDL_SetPointerPropertyWithCleanup(
                    id, name.data(), ptr,
                    [](void*, void* obj) {
                        delete static_cast<T*>(obj);
                    }, nullptr
                )) {
                    delete ptr;
                    throw Error{};
                }
                return *this;
            }

            template<typename T>
            requires std::is_class_v<T>
                && std::copy_constructible<T>
            Property& operator = (const T& value) {
                if (auto ptr = new T{value};
                    !SDL_SetPointerPropertyWithCleanup(
                    id, name.data(), ptr,
                    [](void*, void* obj) {
                        delete static_cast<T*>(obj);
                    }, nullptr
                )) {
                    delete ptr;
                    throw Error{};
                }
                return *this;
            }

            template<typename T, typename ...Args>
            requires std::is_class_v<T>
                && std::constructible_from<T, Args...>
            T& emplace(Args&& args...) {
                auto ptr = new T{std::forward<Args>(args)...};
                if (!SDL_SetPointerPropertyWithCleanup(
                    id, name.data(), ptr,
                    [](void*, void* obj) {
                        delete static_cast<T*>(obj);
                    }, nullptr
                )) {
                    delete ptr;
                    throw Error{};
                }
                return *ptr;
            }

            template<typename T>
            Property& operator = (T* value) {
                if (!SDL_SetPointerProperty(
                    id, name.data(), value
                    ))
                    throw Error{};
                return *this;
            }

            Property& operator = (const std::string_view view) {
                if (!SDL_SetStringProperty(id, name.data(), view.data()))
                    throw Error{};
                return *this;
            }

            Property& operator = (const Sint64 number) {
                if (!SDL_SetNumberProperty(id, name.data(), number))
                    throw Error{};
                return *this;
            }

            Property& operator = (const float number) {
                if (!SDL_SetFloatProperty(id, name.data(), number))
                    throw Error{};
                return *this;
            }

            Property& operator = (const bool number) {
                if (!SDL_SetBooleanProperty(id, name.data(), number))
                    throw Error{};
                return *this;
            }

            bool exists() const {
                return SDL_HasProperty(id, name.data());
            }

            Type type() const {
                return static_cast<Type>(SDL_GetPropertyType(id, name.data()));
            }


            template<typename T>
            T get_or(const T& fallback) const {
                auto name = name.c_str();
                if constexpr (std::is_pointer_v<T>)
                    return static_cast<T*>(SDL_GetPointerProperty(id, name, fallback));
                else if constexpr (std::is_same_v<T, string_t>)
                    return SDL_GetStringProperty(id, name, fallback.c_str());
                else if constexpr (std::is_same_v<T, Sint64>)
                    return SDL_GetSint64Property(id, name, fallback);
                else if constexpr (std::is_same_v<T, float>)
                    return SDL_GetFloatProperty(id, name, fallback);
                else if constexpr (std::is_same_v<T, bool>)
                    return SDL_GetBoolProperty(id, name, fallback);
                else {
                    auto ret = SDL_GetPointerProperty(id, name, nullptr);
                    return ret ? *static_cast<T*>(ret) : fallback;
                }
            }

            void erase() {
                if (!SDL_ClearProperty(id, name.data))
                    throw Error{};
            }
        };


        SDL_PropertiesID
            handle{0};

        explicit Properties(SDL_PropertiesID handle):
            handle{handle} {
            if (!handle)
                throw Error{};
        }

        Properties() noexcept = default;

        Properties(const Properties& other):
            Properties() {
            *this = other;
        }
        Properties& operator = (const Properties& other) {
            if (this == &other)
                return *this;
            if (!SDL_CopyProperties(other.handle, handle))
                throw Error{};
            return *this;
        }

        Properties(Properties&& other) noexcept {
            std::swap(handle, other.handle);
        }
        Properties& operator = (Properties&& other) noexcept {
            if (this == &other)
                return *this;
            std::swap(handle, other.handle);
            return *this;
        }

        void lock() {
            if (!SDL_LockProperties(handle))
                throw Error{};
        }

        void unlock() noexcept {
            SDL_UnlockProperties(handle);
        }

        void transection(const std::function<void(Properties&)>& operation) {
            lock();
            try {
                operation(*this);
                unlock();
            } catch (Error&) {
                unlock();
                throw;
            }
        }

        void visit(const PropertyVisitor visitor) {
            if (!SDL_EnumerateProperties(handle, visitor, nullptr))
                throw Error{};
        }

        ~Properties() {
            if (!handle && handle != SDL_GetGlobalProperties())
                SDL_DestroyProperties(handle);
        }

        static Properties global() {
            if (const auto id = SDL_GetGlobalProperties();
                !id)
                throw Error{};
            else
                return Properties{id};
        }
        static Properties create() {
            return {SDL_CreateProperties()};
        }
    };
}

#endif //SDL_PROPERTIES_HPP
