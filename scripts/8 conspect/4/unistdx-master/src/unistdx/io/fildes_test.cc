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

#include <set>

#include <unistdx/bits/for_each_file_descriptor>
#include <unistdx/io/fildes>
#include <unistdx/io/pipe>

#include <unistdx/test/language>
#include <unistdx/test/sanitize_file_descriptors>
#include <unistdx/test/temporary_file>

using namespace sys::test::lang;

void test_for_each_file_descriptor_pipe() {
    sys::test::file_descriptor_sanitizer fds;
    sys::pipe p;
    std::set<sys::fd_type> expected{p.in().fd(), p.out().fd()}, actual;
    sys::bits::for_each_file_descriptor(
        [&] (const sys::poll_event& rhs) {
            const auto fd = rhs.fd();
            if (fd == p.in().fd() || fd == p.out().fd()) {
                actual.emplace(fd);
            }
        }
    );
    expect(value(expected) == value(actual));
}

void test_fildes_duplicate() {
    sys::test::file_descriptor_sanitizer fds;
    sys::fildes f(UNISTDX_TMPFILE, sys::open_flag::create, 0644);
    sys::fildes tmp1(f);
    sys::fildes tmp2(1000);
    tmp2 = f;
}

void test_fildes_basic() {
    sys::test::file_descriptor_sanitizer fds;
    sys::fildes a, b;
    expect(!value(a));
    expect(!value(b));
    expect(value(a) == value(b));
    expect(value(a) == value(b.fd()));
    expect(value(a.fd()) == value(b));
    b.open(UNISTDX_TMPFILE, sys::open_flag::create, 0644);
    expect(static_cast<bool>(b));
    expect(value(a) != value(b));
    expect(value(a.fd()) != value(b));
    expect(value(a) != value(b.fd()));
}

void test_fildes_traits() {
    sys::test::file_descriptor_sanitizer fds;
    typedef sys::streambuf_traits<sys::fildes> traits_type;
    sys::fildes a;
    char buf[1024] = {0};
    expect(throws<sys::bad_call>(call([&] () { traits_type::read(a, buf, 1024); })));
    expect(throws<sys::bad_call>(call([&] () { traits_type::write(a, buf, 1024); })));
}

void test_fd_type_traits() {
    sys::test::file_descriptor_sanitizer fds;
    typedef sys::streambuf_traits<sys::fd_type> traits_type;
    sys::fildes a;
    char buf[1024] = {0};
    expect(throws<sys::bad_call>(call([&] () { traits_type::read(a.fd(), buf, 1024); })));
    expect(throws<sys::bad_call>(call([&] () { traits_type::write(a.fd(), buf, 1024); })));
    a.open("/dev/null");
    expect(static_cast<bool>(a));
    expect(no_throw(call([&] () { traits_type::read(a.fd(), buf, 1024); })));
}
