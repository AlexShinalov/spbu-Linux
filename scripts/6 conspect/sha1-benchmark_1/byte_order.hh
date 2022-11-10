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

#ifndef UNISTDX_NET_BYTE_ORDER
#define UNISTDX_NET_BYTE_ORDER

#include "byte_swap.hh"

namespace sys {

    /**
    \brief Returns true if this archiecture uses network byte order (little-endian).
    \date 2018-05-23
    \ingroup net
    \details Compile-time constant expression.
    */
    inline constexpr bool
    is_network_byte_order() noexcept {
        #if defined(UNISTDX_BIG_ENDIAN)
        return true;
        #else
        return false;
        #endif
    }

    /**
    \brief
    Converts binary representation of \p n in native byte order
    to network byte order if needed.
    \date 2018-05-23
    \ingroup net
    */
    template<class T>
    inline constexpr T
    to_network_format(T n) noexcept {
        return is_network_byte_order() ? n : byte_swap<T>(n);
    }

    /**
    \brief
    Converts binary representation of \p n in network byte order
    to native byte order if needed.
    \date 2018-05-23
    \ingroup net
    */
    template<class T>
    inline constexpr T
    to_host_format(T n) noexcept {
        return is_network_byte_order() ? n : byte_swap<T>(n);
    }

    enum class byte_order {little_endian, big_endian};

    inline constexpr byte_order
    native_byte_order() noexcept {
        #if defined(UNISTDX_BIG_ENDIAN)
        return byte_order::big_endian;
        #else
        return byte_order::little_endian;
        #endif
    }

}

#endif // vim:filetype=cpp
