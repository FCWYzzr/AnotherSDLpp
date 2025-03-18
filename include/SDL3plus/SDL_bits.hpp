//
// Created by FCWY on 25-3-18.
//

#ifndef SDL_BITS_HPP
#define SDL_BITS_HPP
#include <SDL3/SDL_bits.h>

#include "SDL_stdinc.hpp"

namespace SDL {
    consteval bool HasExactlyOneBitSet32(const Uint32 x) {
        if (x && !(x & (x - 1))) {
            return true;
        }
        return false;
    }

    consteval bool MostSignificantBitIndex32(const Uint32 x) {
        #if defined(__GNUC__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
        /* Count Leading Zeroes builtin in GCC.
         * http://gcc.gnu.org/onlinedocs/gcc-4.3.4/gcc/Other-Builtins.html
         */
        if (x == 0) {
            return -1;
        }
        return 31 - __builtin_clz(x);
        #elif defined(__WATCOMC__) && defined(__386__)
        if (x == 0) {
            return -1;
        }
        return _SDL_bsr_watcom(x);
        #elif defined(_MSC_VER) && _MSC_VER >= 1400
        unsigned long index;
        if (_BitScanReverse(&index, x)) {
            return (int)index;
        }
        return -1;
        #else
        /* Based off of Bit Twiddling Hacks by Sean Eron Anderson
         * <seander@cs.stanford.edu>, released in the public domain.
         * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
         */
        const Uint32 b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
        const int    S[] = {1, 2, 4, 8, 16};

        int msbIndex = 0;
        int i;

        if (x == 0) {
            return -1;
        }

        for (i = 4; i >= 0; i--)
        {
            if (x & b[i])
            {
                x >>= S[i];
                msbIndex |= S[i];
            }
        }

        return msbIndex;
        #endif
    }
}

#endif //SDL_BITS_HPP
