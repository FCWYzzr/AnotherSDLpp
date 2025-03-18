//
// Created by FCWY on 25-3-18.
//

#ifndef SDL_BLENDMODE_HPP
#define SDL_BLENDMODE_HPP
#include <SDL3/SDL_blendmode.h>

#include "SDL_stdinc.hpp"

namespace SDL {
    enum class BlendMode: Uint32 {
        None = 0x00000000u,
        Blend = 0x00000001u,
        BLEND_PREMULTIPLIED = 0x00000010u,
        ADD   =               0x00000002u,
        ADD_PREMULTIPLIED   = 0x00000020u,
        MOD                 = 0x00000004u,
        MUL                 = 0x00000008u,
        INVALID             = 0x7FFFFFFFu
    };
    constexpr SDL_BlendMode legacy(const BlendMode mode) noexcept {
        return static_cast<SDL_BlendMode>(mode);
    }

    enum class BlendOperation {
        ADD = SDL_BLENDOPERATION_ADD,
        SUBTRACT = SDL_BLENDOPERATION_SUBTRACT,
        REV_SUBTRACT = SDL_BLENDOPERATION_REV_SUBTRACT,
        MINIMUM = SDL_BLENDOPERATION_MINIMUM,
        MAXIMUM = SDL_BLENDOPERATION_MAXIMUM
    };
    constexpr SDL_BlendOperation legacy(const BlendOperation mode) noexcept {
        return static_cast<SDL_BlendOperation>(mode);
    }

    enum class BlendFactor {
        ZERO = SDL_BLENDFACTOR_ZERO,
        ONE = SDL_BLENDFACTOR_ONE,
        SRC_COLOR = SDL_BLENDFACTOR_SRC_COLOR,
        ONE_MINUS_SRC_COLOR = SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
        SRC_ALPHA = SDL_BLENDFACTOR_SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA = SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA ,
        DST_COLOR = SDL_BLENDFACTOR_DST_COLOR,
        ONE_MINUS_DST_COLOR = SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
        DST_ALPHA = SDL_BLENDFACTOR_DST_ALPHA,
        ONE_MINUS_DST_ALPHA = SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA
    };
    constexpr SDL_BlendFactor legacy(const BlendFactor factor) noexcept {
        return static_cast<SDL_BlendFactor>(factor);
    }

    inline BlendMode ComposeCustomBlendMode(
        const BlendFactor srcColorFactor,
        const BlendFactor dstColorFactor,
        const BlendOperation colorOperation,
        const BlendFactor srcAlphaFactor,
        const BlendFactor dstAlphaFactor,
        const BlendOperation alphaOperation
    ) noexcept {
        return static_cast<BlendMode>(SDL_ComposeCustomBlendMode(
            legacy(srcColorFactor),
            legacy(dstColorFactor),
            legacy(colorOperation),
            legacy(srcAlphaFactor),
            legacy(dstAlphaFactor),
            legacy(alphaOperation)
        ));
    }
}

#endif //SDL_BLENDMODE_HPP
