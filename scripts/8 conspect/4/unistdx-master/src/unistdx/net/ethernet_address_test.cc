/*
UNISTDX — C++ library for Linux system calls.
© 2019, 2020 Ivan Gankevich

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

#include <unistdx/net/ethernet_address>
#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/operator>
#include <unistdx/test/properties>

using namespace sys::test::lang;

void test_ethernet_address_calculus() {
    using namespace sys::test;
    using octet = sys::ethernet_address::value_type;
    falsify(
        [] (const Argument_array<12>& params) {
            sys::ethernet_address a(params[0], params[1], params[2],
                params[3], params[4], params[5]);
            sys::ethernet_address b(params[6], params[7], params[8],
                params[9], params[10], params[11]);
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>(),
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>(),
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>(),
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>());
}

typedef decltype(std::right) manipulator;

void do_test_print(
    const char* expected,
    sys::ethernet_address addr,
    manipulator manip = std::left,
    size_t width=17
) {
    std::stringstream str;
    str << std::setw(width) << manip << addr;
    std::string result = str.str();
    expect(value(width) == value(result.size()));
    expect(value(expected) == value(result));
}

void test_ethernet_address_print_padding() {
    using namespace sys;
    do_test_print("00:00:00:00:00:00", ethernet_address{});
    do_test_print("ff:ff:ff:ff:ff:ff", ethernet_address{0xff,0xff,0xff,0xff,0xff,0xff});
    do_test_print(" 00:00:00:00:00:00", ethernet_address{}, std::right, 18);
    do_test_print("00:00:00:00:00:00 ", ethernet_address{}, std::left, 18);
}

void test_ethernet_address_io() {
    using namespace sys::test;
    using octet = sys::ethernet_address::value_type;
    falsify(
        [] (const Argument_array<6>& params) {
            sys::ethernet_address addr(params[0], params[1], params[2],
                params[3], params[4], params[5]);
            test::io_operators(addr);
        },
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>(),
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>());
    falsify(
        [] (const Argument_array<6>& params) {
            sys::ethernet_address addr(params[0], params[1], params[2],
                params[3], params[4], params[5]);
            test::bstream_insert_extract(addr);
        },
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>(),
        make_parameter<octet>(), make_parameter<octet>(), make_parameter<octet>());
}
