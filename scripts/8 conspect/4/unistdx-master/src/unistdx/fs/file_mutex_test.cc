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

#include <chrono>
#include <cstdlib>
#include <mutex>
#include <thread>

#include <unistdx/fs/file_mutex>
#include <unistdx/io/event_file_descriptor>
#include <unistdx/ipc/process>
#include <unistdx/test/language>
#include <unistdx/test/temporary_file>

using namespace sys::test::lang;

void test_file_mutex_check() {
    using mutex_type = sys::file_mutex;
    using lock_type = std::lock_guard<mutex_type>;
    test::temporary_file tmp(UNISTDX_TMPFILE);
    mutex_type mtx(tmp.path(), 0600);
    expect(static_cast<bool>(mtx));
    sys::event_file_descriptor notifier(
        0, sys::event_file_descriptor::flag::close_on_exec);
    sys::process child([&tmp,&notifier] () {
        mutex_type mtx2(tmp.path(), 0600);
        lock_type lock(mtx2);
        notifier.write(1);
        ::pause();
        return 0;
    });
    notifier.read();
    expect(!mtx.try_lock());
    child.terminate();
    child.join();
    expect(mtx.try_lock());
    expect(no_throw(call([&] () { mtx.lock(); })));
    expect(no_throw(call([&] () { mtx.unlock(); })));
}

void test_file_mutex_bad_lock() {
    sys::file_mutex mtx;
    expect(no_throw(call([&] () { mtx.open(UNISTDX_TMPFILE, 600); })));
    expect(no_throw(call([&] () { mtx.close(); })));
    expect(no_throw(call([&] () { sys::remove(UNISTDX_TMPFILE); })));
    expect(throws(call([&] () { mtx.try_lock(); })));
    expect(throws(call([&] () { mtx.lock(); })));
    expect(throws(call([&] () { mtx.unlock(); })));
}
