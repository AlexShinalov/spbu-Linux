/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

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

#include <unistdx/base/flag>
#include <unistdx/test/language>

using namespace sys::test::lang;

namespace sys {

    enum class My_flag {A=1, B=2, C=4};

    template <> struct is_flag<My_flag>: public std::true_type {};

}

void test_is_flag() {
    using sys::My_flag;
    My_flag f = My_flag::A | My_flag::B;
    expect(bool(f & My_flag::A));
    expect(bool(f & My_flag::B));
    expect(!bool(f & My_flag::C));
    f |= My_flag::C;
    expect(bool(f & My_flag::A));
    expect(bool(f & My_flag::B));
    expect(bool(f & My_flag::C));
    f &= ~My_flag::B;
    expect(bool(f & My_flag::A));
    expect(!bool(f & My_flag::B));
    expect(bool(f & My_flag::C));
}

namespace another_ns {
    enum class Another_flag {A=1, B=2, C=4};
    UNISTDX_FLAGS(Another_flag)
}

void test_flags_macro() {
    using another_ns::Another_flag;
    Another_flag f = Another_flag::A | Another_flag::B;
    expect(bool(f & Another_flag::A));
    expect(bool(f & Another_flag::B));
    expect(!bool(f & Another_flag::C));
    f |= Another_flag::C;
    expect(bool(f & Another_flag::A));
    expect(bool(f & Another_flag::B));
    expect(bool(f & Another_flag::C));
    f &= ~Another_flag::B;
    expect(bool(f & Another_flag::A));
    expect(!bool(f & Another_flag::B));
    expect(bool(f & Another_flag::C));
}
