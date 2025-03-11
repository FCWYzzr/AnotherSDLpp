//
// Created by FCWY on 25-3-11.
//

// ReSharper disable CppMemberFunctionMayBeConst
#ifndef SDL_ATOMIC_HPP
#define SDL_ATOMIC_HPP
#include "SDL3/SDL_atomic.h"
#include "SDL_stdinc.hpp"

namespace SDL3::plus {
    struct SpinLock {
        SDL_SpinLock*
            handle;

        bool try_lock() noexcept {
            return SDL_TryLockSpinlock(handle);
        }
        void lock() noexcept {
            SDL_LockSpinlock(handle);
        }
        void unlock() noexcept {
            SDL_UnlockSpinlock(handle);
        }
    };

    struct MemoryBarrier {
        constexpr MemoryBarrier() noexcept {
            SDL_MemoryBarrierAcquireFunction();
        }
        constexpr ~MemoryBarrier() noexcept {
            SDL_MemoryBarrierReleaseFunction();
        }
    };

    namespace Atomic {
        // use std::atomic instead
    }
}

#endif //SDL_ATOMIC_HPP
