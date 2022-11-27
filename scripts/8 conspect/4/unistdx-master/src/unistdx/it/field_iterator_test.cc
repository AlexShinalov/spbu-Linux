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

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include <unistdx/it/field_iterator>
#include <unistdx/it/iterator_pair>
#include <unistdx/test/language>

using namespace sys::test::lang;

template <class Container>
void
test_field_iterator() {
    typedef Container container_type;
    typedef sys::field_iterator<typename container_type::iterator,0>
        key_iterator;
    typedef sys::field_iterator<typename container_type::iterator,1>
        value_iterator;
    container_type data = {
        {"a", 1},
        {"b", 2}
    };
    std::vector<std::string> expected_keys = {"a", "b"}, keys;
    std::vector<int> expected_values = {1, 2}, values, values2;
    std::copy(
        key_iterator(data.begin()),
        key_iterator(data.end()),
        std::back_inserter(keys)
    );
    expect(value(expected_keys) == value(keys));
    std::copy(
        value_iterator(data.begin()),
        value_iterator(data.end()),
        std::back_inserter(values)
    );
    expect(value(expected_values) == value(values));
    for (const auto& v : sys::make_view<1>(data)) {
        values2.push_back(v);
    }
    expect(value(expected_values) == value(values2));
}

void test_fielditerator_traverse_vector() {
    test_field_iterator<std::vector<std::pair<std::string,int>>>();
}

void test_fielditerator_traverse_map() {
    test_field_iterator<std::map<std::string,int>>();
}
