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

#include <string>
#include <unistdx/fs/path>
#include <unordered_set>

#include <unistdx/test/language>

using namespace sys::test::lang;

void test_Path_Hash() {
    std::unordered_set<sys::path> s;
    for (int i=0; i<10; ++i) {
        s.emplace("/tmp");
    }
    expect(value(1u) == value(s.size()));
}

void test_Path_Equals() {
    sys::path p("/tmp");
    sys::path q("/tmpx");
    // ==
    expect(value(p) == value(p));
    expect(value("/tmp") == value(p));
    expect(value(p) == value("/tmp"));
    expect(value(std::string("/tmp")) == value(p));
    expect(value(p) == value(std::string("/tmp")));
    // !=
    expect(value(q) != value(p));
    expect(value(p) != value(q));
    expect(value("/tmpx") != value(p));
    expect(value(p) != value("/tmpx"));
    expect(value(std::string("/tmpx")) != value(p));
    expect(value(p) != value(std::string("/tmpx")));
}

void test_Path_VariadicConstructor() {
    expect(value("a") == value(sys::path("a")));
    expect(value("a/b") == value(sys::path("a", "b")));
    expect(value("a/b/c") == value(sys::path("a", "b", "c")));
    expect(value("a/b/c/d") == value(sys::path("a", "b", "c", "d")));
}
