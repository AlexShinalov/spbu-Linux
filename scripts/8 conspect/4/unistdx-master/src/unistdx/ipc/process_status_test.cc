/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020, 2021 Ivan Gankevich

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

#include <chrono>
#include <thread>

#include <unistdx/ipc/process>

#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_process_status_exit() {
    sys::process child {
        [] () {
            return 0;
        }
    };
    sys::process_status status = child.wait();
    expect(status.exited());
    expect(!value(status.killed()));
    expect(!value(status.stopped()));
    expect(!value(status.core_dumped()));
    expect(!value(status.trapped()));
    expect(!value(status.continued()));
    expect(value(0) == value(status.exit_code()));
    expect(value(std::string("exited")) == value(status.status_string()));
    test::stream_insert_contains("status=exited,exit_code=0", status);
}

void test_process_status_abort() {
    sys::process child {
        [] () -> int {
            sys::this_process::bind_signal(sys::signal::abort, SIG_DFL);
            std::abort();
        }
    };
    sys::process_status status = child.wait();
    expect(!value(status.exited()));
    expect(!value(status.stopped()));
    expect(value(status.killed()) || value(status.core_dumped()));
    expect(!value(status.trapped()));
    expect(value(sys::signal::abort) == value(status.term_signal()));
//	EXPECT_STREQ("core_dumped", status.status_string());
    test::stream_insert_contains("signal=abort", status);
}

void
print(const char* str, int si_code) {
    sys::siginfo_type s{};
    s.si_code = si_code;
    sys::process_status status(s);
    test::stream_insert_contains(str, status);
}

void test_process_status_print() {
    print("status=stopped", CLD_STOPPED);
    print("status=continued", CLD_CONTINUED);
    print("status=trapped", CLD_TRAPPED);
}
