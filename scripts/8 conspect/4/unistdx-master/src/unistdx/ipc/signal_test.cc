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

#include <unistdx/ipc/process>
#include <unistdx/ipc/signal>

#include <unistdx/test/exception>
#include <unistdx/test/operator>

using namespace sys::test::lang;

volatile int caught = 0;

void
catch_signal(int) {
    caught = 1;
}

void
catch_signal_2(int) {
}

void
catch_sigaction(int,sys::siginfo_type*,void*) {
    caught = 1;
}

void test_Signal_BindSignal() {
    using namespace sys::this_process;
    bind_signal(sys::signal::user_defined_1, catch_sigaction);
    sys::this_process::send(sys::signal::user_defined_1);
    expect(value(1) == value(caught));
}


void test_signal_print() {
    test::stream_insert_starts_with("terminate", sys::signal::terminate);
    test::stream_insert_starts_with("unknown", sys::signal(-1));
    test::stream_insert_starts_with("unknown", sys::signal(0));
    test::stream_insert_starts_with(
        "unknown",
        sys::signal(std::numeric_limits<sys::signal_type>::max())
    );
}

void test_signal_bind_bad_signal() {
    using namespace sys::this_process;
    UNISTDX_EXPECT_ERROR(
        std::errc::invalid_argument,
        bind_signal(sys::signal(-1), catch_signal_2)
    );
    UNISTDX_EXPECT_ERROR(
        std::errc::invalid_argument,
        bind_signal(sys::signal(-1), catch_sigaction)
    );
}
