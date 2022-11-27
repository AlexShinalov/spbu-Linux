/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020, 2021 Ivan Gankevich

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

#include <unistdx/ipc/futex>
#include <unistdx/test/language>
#include <unistdx/test/mutex>
#include <unistdx/test/semaphore>

void test_futex() {
    using namespace sys::test::lang;
    sys::futex mutex{1};
    expect(value(mutex.try_lock()) == value(true));
    sys::futex semaphore{0};
    expect(value(semaphore.try_lock()) == value(false));
}

void test_futex_as_mutex() {
    struct futex_mutex: public sys::futex {
        futex_mutex(): sys::futex{1} {}
    };
    test::thread<futex_mutex>();
    test::thread_counter<futex_mutex>();
}

void test_futex_as_condition_variable() {
    struct futex_condition_variable: public sys::futex {
        futex_condition_variable(): sys::futex{0} {}
    };
    test::semaphore<futex_condition_variable>();
    test::semaphore_producer_consumer_thread<futex_condition_variable>();
    test::semaphore_wait_until<futex_condition_variable>();
}
