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

#include <fstream>
#include <iostream>
#include <vector>

#include <unistdx/test/language>
#include <unistdx/test/random_string>
#include <unistdx/test/temporary_file>

#include <unistdx/io/memory_mapping>

using namespace sys::test::lang;

void test_memory_mapping_anonymous() {
    sys::memory_mapping<char> anon{1024};
}

void test_memory_mapping_file() {
    test::temporary_file tmp(UNISTDX_TMPFILE);
    std::string expected_contents = test::random_string<char>(3333);
    { std::ofstream{tmp.path()} << expected_contents; }
    sys::memory_mapping<char> mapping(tmp.path());
    std::string actual{mapping.begin(), mapping.end()};
    expect(value(expected_contents) == value(actual));
}
