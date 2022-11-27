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

#include <mutex>

#include <unistdx/ipc/process_group>
#include <unistdx/test/config>
#include <unistdx/test/operator>
#include <valgrind/config>

using namespace sys::test::lang;

using f = sys::process_flag;

void test_process_group_wait_async() {
    UNISTDX_SKIP_IF_RUNNING_ON_VALGRIND();
    sys::process_group g;
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    std::mutex mtx;
    g.wait(
        mtx,
        [] (const sys::process&, sys::process_status status) {
            expect(status.exited());
            expect(!value(status.killed()));
            expect(!value(status.stopped()));
            expect(!value(status.core_dumped()));
            expect(!value(status.trapped()));
            expect(!value(status.continued()));
            expect(value(0) == value(status.exit_code()));
            expect(value(std::string("exited")) == value(status.status_string()));
            expect(value("") != value(test::stream_insert(status)));
        }
    );
}

void test_process_group_wait_sync() {
    UNISTDX_SKIP_IF_RUNNING_ON_VALGRIND();
    sys::process_group g;
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    int ret = g.wait();
    expect(value(0) == value(ret));
}
