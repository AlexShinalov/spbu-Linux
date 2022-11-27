/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <unistdx/net/bytes>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_bytes_members() {
    const sys::bytes<sys::u32> b(0xff);
    expect(value(sizeof(sys::u32)) == value(b.size()));
    expect(value(size_t(b.end() - b.begin())) == value(b.size()));
    expect(value(0xffu) == value(b.value()));
    sys::u32 v = b;
    expect(value(0xffu) == value(v));
}

void test_bytes_front_begin() {
    sys::bytes<sys::u32> b;
    for (int i=0; i<4; ++i) {
        b[i] = i+1;
    }
    expect(value(1) == value(b.front()));
    expect(value(4) == value(b.back()));
    sys::bytes<sys::u32> copy(b);
    b.to_network_format();
    if (sys::is_network_byte_order()) {
        expect(value(copy) == value(b));
    } else {
        expect(value(copy) != value(b));
        expect(value(4) == value(b[0]));
        expect(value(3) == value(b[1]));
        expect(value(2) == value(b[2]));
        expect(value(1) == value(b[3]));
    }
    b.to_host_format();
    expect(value(copy) == value(b));
    test::stream_insert_equals("01 02 03 04", b);
}

void test_bytes_constructors() {
    sys::bytes<sys::u64> a{};
    sys::bytes<sys::u64> b(0);
    expect(value(a) == value(b));
    char data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    sys::bytes<sys::u64> c(std::begin(data), std::end(data));
    expect(value(1) == value(c.front()));
    expect(value(8) == value(c.back()));
}
