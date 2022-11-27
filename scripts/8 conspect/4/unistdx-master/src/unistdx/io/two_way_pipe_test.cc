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

#include <unistdx/io/two_way_pipe>

#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_two_way_pipe_print() {
    sys::two_way_pipe p;
    expect(value("") != value(test::stream_insert(p)));
}

void test_two_way_pipe_close_in_parent() {
    sys::two_way_pipe p;
    expect(no_throw(call([&] () { p.validate(); })));
    p.close_in_parent();
    expect(no_throw(call([&] () { p.validate(); })));
}

void test_two_way_pipe_close_unused() {
    sys::two_way_pipe p;
    expect(no_throw(call([&] () { p.validate(); })));
    p.close_unused();
    expect(no_throw(call([&] () { p.validate(); })));
}

void test_two_way_pipe_close() {
    sys::two_way_pipe p;
    expect(no_throw(call([&] () { p.validate(); })));
    p.close();
    expect(throws<sys::bad_call>(call([&] () { p.validate(); })));
}

void test_two_way_pipe_close_in_child() {
    sys::two_way_pipe p;
    expect(no_throw(call([&] () { p.validate(); })));
    p.close_in_child();
    expect(throws<sys::bad_call>(call([&] () { p.validate(); })));
}

void test_two_way_pipe_child_close_in_child() {
    sys::two_way_pipe p;
    expect(no_throw(call([&] () { p.validate(); })));
    sys::process child{
        [&p] () {
            int ret = 0;
            try {
                p.close_in_child();
            } catch (...) {
                ++ret;
            }
            try {
                p.validate();
            } catch (...) {
                ++ret;
            }
            return ret;
        }
    };
    p.close_in_parent();
    expect(no_throw(call([&] () { p.validate(); })));
    sys::process_status status = child.wait();
    expect(value(0) == value(status.exit_code()));
}

void test_two_way_pipe_child_close_unused() {
    sys::two_way_pipe p;
    expect(no_throw(call([&] () { p.validate(); })));
    sys::process child{
        [&p] () {
            int ret = 0;
            try {
                p.close_unused();
            } catch (...) {
                ++ret;
            }
            try {
                p.validate();
            } catch (...) {
                ++ret;
            }
            return ret;
        }
    };
    p.close_in_parent();
    expect(no_throw(call([&] () { p.validate(); })));
    sys::process_status status = child.wait();
    expect(value(0) == value(status.exit_code()));
}

void test_two_way_pipe_open() {
    sys::two_way_pipe p;
    expect(no_throw(call([&] () { p.validate(); })));
    p.open();
    expect(no_throw(call([&] () { p.validate(); })));
}
