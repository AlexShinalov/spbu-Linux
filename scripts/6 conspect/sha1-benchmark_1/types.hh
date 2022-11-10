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

#ifndef UNISTDX_BASE_TYPES
#define UNISTDX_BASE_TYPES

#include <cstddef>
#include <cstdint>

namespace sys {

    /// 1-byte unsigned integer type.
    typedef ::std::uint8_t u8;
    /// 2-byte unsigned integer type.
    typedef ::std::uint16_t u16;
    /// 4-byte unsigned integer type.
    typedef ::std::uint32_t u32;
    /// 8-byte unsigned integer type.
    typedef ::std::uint64_t u64;

    /// 1-byte signed integer type.
    typedef ::std::int8_t i8;
    /// 2-byte signed integer type.
    typedef ::std::int16_t i16;
    /// 4-byte signed integer type.
    typedef ::std::int32_t i32;
    /// 8-byte signed integer type.
    typedef ::std::int64_t i64;

    /// 4-byte floating point type.
    typedef float f32;
    /// 8-byte floating point type.
    typedef double f64;

}

#endif // vim:filetype=cpp
