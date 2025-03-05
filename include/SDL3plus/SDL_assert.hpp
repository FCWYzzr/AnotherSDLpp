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
    using AssertionHandler = SDL_AssertState (SDLCALL *)(const AssertData *, void *userdata) noexcept;

    inline void SetAssertionHandler(AssertionHandler handler, void* data) noexcept {
        return SDL_SetAssertionHandler(handler, data);
    }

    inline AssertionHandler GetDefaultAssertionHandler() noexcept {
        return reinterpret_cast<AssertionHandler>(SDL_GetDefaultAssertionHandler());
    }

    inline AssertionHandler GetAssertionHandler(void **puserdata) noexcept {
        return reinterpret_cast<AssertionHandler>(SDL_GetAssertionHandler(puserdata));
    }

    inline const AssertData* GetAssertionReport() noexcept {
        return SDL_GetAssertionReport();
    }

    inline void ResetAssertionReport() noexcept {
        return SDL_ResetAssertionReport();
    }
}

#endif //SDL_ASSERT_HPP
