/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <iterator>
#include <string>
#include <vector>

#include <unistdx/it/cstring_iterator>
#include <unistdx/test/language>

using namespace sys::test::lang;

void test_cstring_iterator_basic() {
    const char* args[] = {
        "first",
        "second",
        nullptr
    };
    sys::cstring_iterator<const char*> first(args), last;
    std::vector<std::string> actual;
    std::copy(first, last, std::back_inserter(actual));
    expect(value(2u) == value(actual.size()));
    expect(value("first") == value(actual[0]));
    expect(value("second") == value(actual[1]));
}

struct args_and_count {

    size_t count;
    std::vector<const char*> args;

    inline const char**
    get_args() const noexcept {
        return const_cast<const char**>(args.data());
    }

};

void test_cstring_iterator() {
    using iterator = sys::cstring_iterator<const char*>;
    for (const auto& param : {
        args_and_count{2, {"1", "2", 0}},
        args_and_count{1, {"1", 0}},
        args_and_count{0, {0}}})
    {
        const size_t cnt = std::distance(iterator(param.get_args()), iterator());
        expect(value(param.count) == value(cnt));
    }
}
