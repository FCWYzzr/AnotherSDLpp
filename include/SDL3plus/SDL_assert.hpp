//
// Created by FCWY on 25-3-5.
//

#ifndef SDL_ASSERT_HPP
#define SDL_ASSERT_HPP
#include <SDL3/SDL_assert.h>

namespace SDL::assert {
    // enum class AssertState {
    //     RETRY = SDL_ASSERTION_RETRY,  /**< Retry the assert immediately. */
    //     BREAK = SDL_ASSERTION_BREAK,  /**< Make the debugger trigger a breakpoint. */
    //     ABORT = SDL_ASSERTION_ABORT,  /**< Terminate the program. */
    //     IGNORE = SDL_ASSERTION_IGNORE,  /**< Ignore the assert. */
    //     ALWAYS_IGNORE = SDL_ASSERTION_ALWAYS_IGNORE
    // };
    using AssertState = SDL_AssertState;
    using AssertData = SDL_AssertData;
    using AssertionHandler = SDL_AssertionHandler;

    template<typename T>
    void SetAssertionHandler(const AssertionHandler handler, T* data) noexcept {
        return SDL_SetAssertionHandler(handler, static_cast<void*>(data));
    }

    inline AssertionHandler GetDefaultAssertionHandler() noexcept {
        return reinterpret_cast<AssertionHandler>(SDL_GetDefaultAssertionHandler());
    }

    template<typename T>
    AssertionHandler GetAssertionHandler(T **puserdata) noexcept {
        return SDL_GetAssertionHandler(reinterpret_cast<void**>(puserdata));
    }

    inline const AssertData* GetAssertionReport() noexcept {
        return SDL_GetAssertionReport();
    }

    inline void ResetAssertionReport() noexcept {
        return SDL_ResetAssertionReport();
    }
}

#endif //SDL_ASSERT_HPP
