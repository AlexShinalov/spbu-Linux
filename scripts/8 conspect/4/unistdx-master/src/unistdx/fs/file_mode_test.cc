/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2020 Ivan Gankevich

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

#include <unistdx/fs/file_mode>
#include <unistdx/test/language>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_file_mode_operators() {
    sys::file_mode a;
    sys::file_mode b(0755u);
    sys::file_mode c(0644u);
    sys::file_mode d(0755u);
    sys::file_mode e(c);
    expect(value(a) == value(a));
    expect(value(b) == value(b));
    expect(value(c) == value(c));
    expect(value(a) != value(b));
    expect(value(a) != value(c));
    expect(value(a) != value(d));
    expect(value(b) == value(d));
    expect(value(c) == value(e));
    expect(value(c) == value(0644u));
    expect(value(c.mode()) == value(0644u));
    expect(value(c) != value(0600u));
    c = 0600u;
    expect(value(c) == value(0600u));
}

void test_file_mode_bits() {
    sys::file_mode a(0752u);
    sys::file_mode b(0147u);
    sys::file_mode c(0640u);
    sys::file_mode e(01640u);
    sys::file_mode d(a.user() | b.group() | c.other() | e.special());
    expect(value(sys::file_mode(01740u)) == value(d));
}

void test_file_mode_print() {
    test::stream_insert_equals("--trwxrw-r--", sys::file_mode(01764u));
    test::stream_insert_equals("ug-rwxrw-r--", sys::file_mode(06764u));
    test::stream_insert_equals("------rw-r--", sys::file_mode(0064u));
    test::stream_insert_equals("---r----xr--", sys::file_mode(0414u));
    test::stream_insert_equals("---r----x-wx", sys::file_mode(0413u));
}
