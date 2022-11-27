/*
UNISTDX — C++ library for Linux system calls.
© 2016, 2017, 2018, 2019, 2020 Ivan Gankevich

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
#include <iostream>
#include <iterator>

#include <unistdx/ipc/identity>
#include <unistdx/system/nss>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_user_enumerate() {
    std::exit(77);
    {
        sys::userstream users;
        const ptrdiff_t dist =
            std::distance(sys::user_iterator(users), sys::user_iterator());
        if (!expect(value(dist) >= value(1))) {
            std::cout << "all users" << std::endl;
            sys::userstream users;
            std::copy(
                sys::user_iterator(users),
                sys::user_iterator(),
                std::ostream_iterator<sys::user>(std::cout, "\n"));
        }
    }
}

void test_group_enumerate() {
    std::exit(77);
    {
        sys::groupstream groups;
        const ptrdiff_t dist =
            std::distance(sys::group_iterator(groups), sys::group_iterator());
        if (!expect(value(dist) >= value(1))) {
            std::cout << "all groups" << std::endl;
            sys::groupstream groups;
            std::copy(
                sys::group_iterator(groups),
                sys::group_iterator(),
                std::ostream_iterator<sys::group>(std::cout, "\n"));
        }
    }
}

void test_user_find_by() {
    std::exit(77);
    errno = 0;
    sys::user u1, u2, u3;
    bool success;
    success = sys::find_user(sys::this_process::user(), u1);
    expect(success);
    expect(value(sys::this_process::user()) == value(u1.id()));
    expect(value(sys::this_process::group()) == value(u1.group_id()));
    success = sys::find_user(u1.name(), u2);
    expect(success);
    expect(value(u1) == value(u2));
    expect(value(std::string(u1.password())) == value(u2.password()));
    expect(value(std::string(u1.real_name())) == value(u2.real_name()));
    sys::user copy(std::move(u2));
    expect(value(u1) == value(copy));
    test::stream_insert_starts_with(u1.name(), u1);
    success = sys::find_user("a", u3);
    expect(!success);
}

void test_group_find_by() {
    std::exit(77);
    sys::group g1, g2;
    bool success;
    success = sys::find_group(sys::this_process::group(), g1);
    expect(success);
    expect(value(sys::this_process::group()) == value(g1.id()));
    success = sys::find_group(g1.name(), g2);
    expect(success);
    expect(value(g1) == value(g2));
    expect(value(g1.size()) == value(g2.size()));
    expect(value(std::string(g1.name())) == value(g2.name()));
    expect(value(std::string(g1.password())) == value(g2.password()));
    sys::group copy(std::move(g2));
    expect(value(g1) == value(copy));
    test::stream_insert_starts_with(g1.name(), g1);
}
