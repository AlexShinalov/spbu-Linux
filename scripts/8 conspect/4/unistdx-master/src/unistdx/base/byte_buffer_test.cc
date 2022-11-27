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

#include <unistdx/base/byte_buffer>
#include <unistdx/base/check>
#include <unistdx/test/language>

using namespace sys::test::lang;

void test_byte_buffer_resize() {
    const size_t max0 = sys::byte_buffer::max_size();
    sys::byte_buffer buf(4096);
    expect(throws(call([&] () { buf.resize(max0); })));
    //expect(throws(call([&] () { sys::byte_buffer b(max0); })));
    try {
        sys::byte_buffer b(max0);
    } catch (const sys::bad_call& err) {
        expect(value(err.error()) == value(sys::errors::out_of_memmory));
    }
}

void test_byte_buffer_resize_empty() {
    sys::byte_buffer buf(0);
    buf.resize(4096);
    expect(value(4096u) == value(buf.size()));
}

void test_byte_buffer_read() {
    sys::byte_buffer buf(0);
    char tmp[16];
    expect(throws(call([&] () { buf.read(tmp, sizeof(tmp)); })));
    buf.resize(4096);
    expect(no_throw(call([&] () { buf.read(tmp, sizeof(tmp)); })));
}

void test_byte_buffer_peek() {
    sys::byte_buffer buf(0);
    char tmp[16];
    expect(throws(call([&] () { buf.peek(tmp, sizeof(tmp)); })));
    buf.resize(4096);
    expect(no_throw(call([&] () { buf.peek(tmp, sizeof(tmp)); })));
}

void test_byte_buffer_copy() {
    sys::byte_buffer a(4096);
    a.write(1);
    a.write(2);
    a.write(3);
    sys::byte_buffer b(a);
    expect(std::equal(a.data(), a.data()+a.size(), b.data()));
    sys::byte_buffer c(4096);
    c.write(4);
    c.write(5);
    c.write(6);
    c = a;
    expect(std::equal(a.data(), a.data()+a.size(), c.data()));
}
