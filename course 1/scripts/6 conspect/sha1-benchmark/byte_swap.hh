/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#ifndef UNISTDX_NET_BYTE_SWAP
#define UNISTDX_NET_BYTE_SWAP

#include <utility>

#include "types.hh"

namespace sys {

    /**
    \brief Byte-swapping function.
    \date 2018-05-23
    \ingroup net
    \details
    \arg There are specialisations for standard unsigned integer types.
    \arg Specialisations are compile-time where supported.
    */
    template<class T>
    inline T
    byte_swap(T n) noexcept;

    /// \brief Specialisation for 1-byte unsigned integer.
    /// \ingroup net
    template<>
    inline constexpr u8
    byte_swap<u8>(u8 n) noexcept {
        return n;
    }

    /// \brief Specialisation for 2-byte unsigned integer.
    /// \ingroup net
    template<>
    inline constexpr u16
    byte_swap<u16>(u16 n) noexcept {
        #if defined(UNISTDX_HAVE_BUILTIN_BSWAP16)
        return __builtin_bswap16(n);
        #else
        return ((n & 0xff00)>>8) | ((n & 0x00ff)<<8);
        #endif
    }

    /// \brief Specialisation for 4-byte unsigned integer.
    /// \ingroup net
    template<>
    inline constexpr u32
    byte_swap<u32>(u32 n) noexcept {
        #if defined(UNISTDX_HAVE_BUILTIN_BSWAP32)
        return __builtin_bswap32(n);
        #else
        return ((n & UINT32_C(0xff000000)) >> 24) |
               ((n & UINT32_C(0x00ff0000)) >> 8) |
               ((n & UINT32_C(0x0000ff00)) << 8) |
               ((n & UINT32_C(0x000000ff)) << 24);
        #endif
    }

    /// \brief Specialisation for 8-byte unsigned integer.
    /// \ingroup net
    template<>
    inline constexpr u64
    byte_swap<u64>(u64 n) noexcept {
        #if defined(UNISTDX_HAVE_BUILTIN_BSWAP64)
        return __builtin_bswap64(n);
        #else
        return ((n & UINT64_C(0xff00000000000000)) >> 56) |
               ((n & UINT64_C(0x00ff000000000000)) >> 40) |
               ((n & UINT64_C(0x0000ff0000000000)) >> 24) |
               ((n & UINT64_C(0x000000ff00000000)) >> 8) |
               ((n & UINT64_C(0x00000000ff000000)) << 8) |
               ((n & UINT64_C(0x0000000000ff0000)) << 24) |
               ((n & UINT64_C(0x000000000000ff00)) << 40) |
               ((n & UINT64_C(0x00000000000000ff)) << 56);
        #endif
    }

    #if defined(UNISTDX_HAVE_INT128)
    /// \brief Specialisation for built-in 16-byte unsigned integer.
    /// \ingroup net
    template<>
    inline unsigned __int128
    byte_swap(unsigned __int128 x) noexcept {
        union { unsigned __int128 n{}; u64 words[2]; } tmp;
        tmp.n = x;
        tmp.words[0] = byte_swap<u64>(tmp.words[0]);
        tmp.words[1] = byte_swap<u64>(tmp.words[1]);
        std::swap(tmp.words[0], tmp.words[1]);
        return tmp.n;
    }
    #endif

}

#endif // vim:filetype=cpp
