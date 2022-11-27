/*
UNISTDX — C++ library for Linux system calls.
© 2016, 2017, 2018, 2020, 2021 Ivan Gankevich

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

#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>
#include <vector>

#include <unistdx/ipc/semaphore>

#include <unistdx/test/language>
#include <unistdx/test/mutex>
#include <unistdx/test/semaphore>

using namespace sys::test::lang;

struct posix_process_semaphore: public sys::posix_semaphore {
    inline
    posix_process_semaphore():
    sys::posix_semaphore(sys::semaphore_type::process)
    {}
};

struct posix_thread_semaphore: public sys::posix_semaphore {
    inline
    posix_thread_semaphore():
    sys::posix_semaphore(sys::semaphore_type::thread)
    {}
};

void test_semaphore() {
    test::semaphore<std::condition_variable>();
}

void test_posix_process_semaphore() {
    test::semaphore_is_available<posix_process_semaphore>();
    #if defined(UNISTDX_HAVE_SEMAPHORE_H)
    test::semaphore<posix_process_semaphore>();
    #endif
}

void test_semaphore_wait_until() {
    test::semaphore_wait_until<std::condition_variable>();
}

void test_semaphore_producer_consumer() {
    test::semaphore_producer_consumer_thread<std::condition_variable>();
}

void test_posix_thread_semaphore() {
    #if defined(UNISTDX_HAVE_SEMAPHORE_H)
    test::semaphore_is_available<posix_thread_semaphore>();
    test::semaphore_producer_consumer_thread<posix_thread_semaphore>();
    test::semaphore_wait_until<posix_thread_semaphore>();
    #endif
}
