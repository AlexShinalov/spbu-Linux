/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2019, 2020 Ivan Gankevich

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

#include <stdlib.h>

#include <unistdx/io/terminal>
#include <unistdx/system/resource>
#include <unistdx/test/language>

using namespace sys::test::lang;

void test_system_thread_concurrency() {
    ::setenv("UNISTDX_CONCURRENCY", "123", 1);
    expect(value(123u) == value(sys::thread_concurrency()));
    ::setenv("UNISTDX_CONCURRENCY", "1", 1);
    expect(value(1u) == value(sys::thread_concurrency()));
    ::setenv("UNISTDX_CONCURRENCY", "0", 1);
    expect(value(0u) != value(sys::thread_concurrency()));
    ::setenv("UNISTDX_CONCURRENCY", "-123", 1);
    expect(value(unsigned(-123)) != value(sys::thread_concurrency()));
    ::unsetenv("UNISTDX_CONCURRENCY");
}

void test_system_io_concurrency() {
    expect(value(sys::io_concurrency()) > value(0u));
}

void test_system_page_size() {
    expect(value(sys::page_size()) > value(0u));
}

void test_system_cache() {
    sys::cache cache;
    for (const auto& c : cache) {
        std::clog << "Cache level=" << c.level()
            << ",size=" << c.size()
            << ",line_size=" << c.line_size()
            << ",assoc=" << c.associativity()
            << std::endl;
    }
}
