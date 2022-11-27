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

#include <unistdx/io/fdstream>
#include <unistdx/io/pipe>
#include <unistdx/ipc/process>
#include <unistdx/ipc/signal>
#include <unistdx/system/error>
#include <unistdx/test/config>
#include <valgrind/config>

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

#if defined(__GNUC__)
#define NO_INLINE [[gnu::noinline]]
#else
#define NO_INLINE
#endif

enum struct Test_type {
    Signal,
    Terminate,
    Signal_thread,
    Terminate_thread
};

std::istream&
operator>>(std::istream& in, Test_type& rhs) {
    std::string s;
    in >> s;
    if (s == "signal") {
        rhs = Test_type::Signal;
    } else if (s == "signal_thread") {
        rhs = Test_type::Signal_thread;
    } else if (s == "terminate") {
        rhs = Test_type::Terminate;
    } else if (s == "terminate_thread") {
        rhs = Test_type::Terminate_thread;
    } else {
        throw std::invalid_argument("bad test type");
    }
    return in;
}

void
print_error() {
    sys::pipe p;
    p.in().unsetf(sys::open_flag::non_blocking);
    p.out().unsetf(sys::open_flag::non_blocking);
    sys::backtrace(p.out().fd());
    p.out().close();
    bool func1_found = false;
    bool func2_found = false;
    sys::ifdstream str(std::move(p.in()));
    std::string line;
    while (std::getline(str, line)) {
        if (line.find("func1") != std::string::npos) {
            func1_found = true;
        } else if (line.find("func2") != std::string::npos) {
            func2_found = true;
        }
    }
    int ret = EXIT_SUCCESS;
    if (!func1_found) {
        std::cerr << "func1 not found" << std::endl;
    }
    if (!func2_found) {
        std::cerr << "func2 not found" << std::endl;
    }
    if (!func1_found && !func2_found) {
        ret = EXIT_FAILURE;
        sys::backtrace(STDERR_FILENO);
    }
    // Skip test, as it unreliable in some build environments.
    ret = 77;
    std::exit(ret);
}

void
print_error_signal(int,sys::siginfo_type*,void*) {
    print_error();
}

NO_INLINE void
func2(Test_type type) {
    if (type == Test_type::Terminate) {
        std::set_terminate(print_error);
        throw std::runtime_error("...");
    } else if (type == Test_type::Signal) {
        using namespace sys::this_process;
        bind_signal(sys::signal::user_defined_1, print_error_signal);
        send(sys::signal::user_defined_1);
    } else if (type == Test_type::Terminate_thread) {
        std::set_terminate(print_error);
        sys::backtrace(2);
        throw std::runtime_error("...");
    }
}

NO_INLINE void
func1(Test_type type) {
    func2(type);
}

void do_test_backtrace(Test_type type) {
    UNISTDX_SKIP_IF_RUNNING_ON_VALGRIND();
    if (type == Test_type::Terminate_thread || type == Test_type::Signal_thread) {
        std::thread t(
            [type] () {
                func1(type);
            }
        );
        if (t.joinable()) {
            t.join();
        }
    } else {
        func1(type);
    }
}

void test_backtrace_on_signal() {
    do_test_backtrace(Test_type::Signal);
}

void test_backtrace_on_terminate() {
    do_test_backtrace(Test_type::Terminate);
}
