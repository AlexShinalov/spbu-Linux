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

#include <unistdx/ipc/identity>
#include <unistdx/test/language>

using namespace sys::test::lang;

void test_identity_exceptions_run_as_root() {
    std::exit(77);
    using namespace sys::this_process;
    if (user() != sys::superuser()) { std::exit(77); }
    expect(value(1000u) != value(user()));
    expect(value(1000u) != value(group()));
    set_identity(1000, 1000);
    expect(value(1000u) == value(user()));
    expect(value(1000u) == value(group()));
    expect(value(1000u) == value(effective_user()));
    expect(value(1000u) == value(effective_group()));
}

void test_identity_basic() {
    using namespace sys::this_process;
    expect(value(user()) == value(effective_user()));
    expect(value(group()) == value(effective_group()));
    expect(value(0u) == value(sys::supergroup()));
    expect(value(0u) == value(sys::superuser()));
}

void test_identity_set_unpriviledged() {
    std::exit(77);
    using namespace sys::this_process;
    if (user() == sys::superuser()) { std::exit(77); }
    sys::uid_type olduser = user();
    sys::gid_type oldgroup = group();
    sys::uid_type newuid = user() + 1;
    sys::gid_type newgid = group() + 1;
    expect(throws<sys::bad_call>(call([&] () { set_identity(newuid, newgid); })));
    expect(value(olduser) == value(user()));
    expect(value(oldgroup) == value(group()));
}
