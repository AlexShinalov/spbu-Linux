/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2020 Ivan Gankevich

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

#include <unistdx/fs/copy_file>
#include <unistdx/fs/file_status>
#include <unistdx/io/fildes>

#include <unistdx/test/language>
#include <unistdx/test/random_string>
#include <unistdx/test/temporary_file>
#include <unistdx/test/tmpdir>

using namespace sys::test::lang;

std::vector<size_t> all_sizes{0, 1, 2, 111, 4096, 4097, 10000};

void test_copy_file_test_all() {
    for (auto size : all_sizes) {
        test::tmpdir tmp(UNISTDX_TMPDIR);
        std::clog << "tmp=" << tmp.name() << std::endl;
        sys::path src(tmp.name(), "x");
        sys::path dst(tmp.name(), "y");
        std::ofstream(src) << test::random_string<char>(size);
        sys::copy_file(src, dst);
        std::stringstream orig;
        orig << std::ifstream(src).rdbuf();
        std::stringstream copy;
        copy << std::ifstream(dst).rdbuf();
        expect(value(orig.str()) == value(copy.str()));
        sys::remove(src);
        sys::remove(dst);
    }
}
