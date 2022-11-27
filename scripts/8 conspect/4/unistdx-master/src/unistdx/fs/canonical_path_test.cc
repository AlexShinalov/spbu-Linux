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

#include <string>
#include <unordered_set>

#include <unistdx/fs/canonical_path>
#include <unistdx/system/resource>
#include <unistdx/test/language>

using namespace sys::test::lang;

void test_canonical_path_hash() {
    std::unordered_set<sys::canonical_path> s;
    for (int i=0; i<10; ++i) {
        s.emplace("/tmp");
    }
    expect(value(1u) == value(s.size()));
}

void test_canonical_path_equals_current_working_directory() {
    sys::canonical_path dir1(".");
    char buf[4096*4] = {0};
    expect(value(nullptr) != value(::getcwd(buf, sizeof(buf))));
    sys::canonical_path dir2(buf);
    expect(value(dir1) == value(dir2));
    sys::canonical_path dir3 = sys::this_process::workdir();
    expect(value(dir2) == value(dir3));
}

void test_canonical_path_assign() {
    sys::canonical_path dir1(".");
    sys::canonical_path dir2("..");
    dir1 = dir1.dirname();
    expect(value(dir1) == value(dir2));
}

void test_canonical_path_dirname() {
    sys::canonical_path dir1("/tmp");
    sys::canonical_path dir2("/");
    expect(value(dir1.dirname()) == value(dir2));
    expect(value(dir2.dirname()) == value(dir2));
    expect(value(dir2.basename()) == value(dir2));
}

void test_canonical_path_two_arg_constructor() {
    sys::canonical_path cwd(".");
    expect(value(sys::canonical_path(cwd.dirname(),cwd.basename())) == value(cwd));
}

void test_canonical_path_non_existent() {
    sys::canonical_path root("/");
    sys::canonical_path tmp;
    expect(throws(call([&] () {
        sys::canonical_path nonexistent(root, "non-existent-directory");
    })));
}

void test_canonical_path_from_string() {
    std::string str = "/";
    sys::canonical_path root(str);
}
