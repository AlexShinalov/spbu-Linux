/*
UNISTDX — C++ library for Linux system calls.
© 2020, 2021 Ivan Gankevich

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

#include <unistdx/ipc/semaphore>
#include <unistdx/test/semaphore>

#if defined(UNISTDX_HAVE_SYS_SEM_H)
void test_sysv_semaphore() {
    test::semaphore<sys::sysv_semaphore>();
}

void test_sysv_semaphore_wait_until() {
    test::semaphore_is_available<sys::sysv_semaphore>();
    test::semaphore_wait_until<sys::sysv_semaphore>();
}

void test_sysv_semaphore_producer_consumer_thread() {
    test::semaphore_is_available<sys::sysv_semaphore>();
    test::semaphore_producer_consumer_thread<sys::sysv_semaphore>();
}

void test_sysv_semaphore_producer_consumer_process() {
    using namespace sys::test::lang;
    test::semaphore_is_available<sys::sysv_semaphore>();
    sys::sysv_semaphore sem;
    sys::process child([&sem] () {
        sys::sysv_semaphore sem_child(sem.id());
        sem_child.wait();
    });
    sem.notify_one();
    sys::process_status status = child.wait();
    expect(status.exited());
    expect(value(EXIT_SUCCESS) == value(status.exit_code()));
}
#endif
