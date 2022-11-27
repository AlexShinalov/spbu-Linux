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

#include <unistdx/base/command_line>
#include <unistdx/test/language>

using namespace sys::test::lang;

void test_commandline_parse() {
    const char* argv[] = {
        "progname",
        "arg1=123",
        "arg2=hello"
    };
    int arg1 = 0;
    std::string arg2;
    sys::input_operator_type options[] = {
        sys::ignore_first_argument(),
        sys::make_key_value("arg1", arg1),
        sys::make_key_value("arg2", arg2),
        nullptr
    };
    sys::parse_arguments(3, argv, options);
    expect(value(123) == value(arg1));
    expect(value("hello") == value(arg2));
}

void test_commandline_invalid_argument() {
    const char* argv[] = {
        "progname",
        "arg3=123"
    };
    int arg1 = 0;
    std::string arg2;
    sys::input_operator_type options[] = {
        sys::ignore_first_argument(),
        sys::make_key_value("arg1", arg1),
        sys::make_key_value("arg2", arg2),
        nullptr
    };
    try {
        sys::parse_arguments(2, argv, options);
        expect(false);
    } catch (const sys::bad_argument& err) {
        expect(value(err.what()) != value(nullptr));
    }
    expect(value(0) == value(arg1));
    expect(arg2.empty());
}
