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

#include <unistdx/net/ipv4_address>
#include <unistdx/net/ipv6_address>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_ipv4_address_calculus() {
    using sys::ipv4_address;
    typedef ipv4_address::rep_type rep_type;
    expect(value(rep_type(1)) ==
           value(ipv4_address(127,0,0,1).position(ipv4_address(255,0,0,0))));
    expect(value(rep_type(5)) ==
           value(ipv4_address(127,0,0,5).position(ipv4_address(255,0,0,0))));
    expect(value(ipv4_address(255,255,255,0).to_prefix()) == value(sys::prefix_type(24)));
    expect(value(ipv4_address(255,255,0,0).to_prefix()) == value(sys::prefix_type(16)));
    expect(value(ipv4_address(255,0,0,0).to_prefix()) == value(sys::prefix_type(8)));
}

typedef decltype(std::right) manipulator;

void
print(
    const char* expected,
    sys::ipv4_address addr,
    manipulator manip,
    size_t width
) {
    std::stringstream str;
    str << std::setw(width) << manip << addr;
    std::string result = str.str();
    expect(value(width) == value(result.size()));
    expect(value(expected) == value(str.str()));
}

void test_ipv4_address_print_padding() {
    print(" 127.0.0.1", sys::ipv4_address{127,0,0,1}, std::right, 10);
    print("127.0.0.1 ", sys::ipv4_address{127,0,0,1}, std::left, 10);
}

void test_ipv4_address_identities() {
    expect(value(sys::ipv4_address{}) == value(sys::ipv4_address{"0.0.0.0"}));
    expect(value(sys::ipv4_address(0,0,0,0)) == value(sys::ipv4_address{"0.0.0.0"}));
}

void test_ipv4_address_ordering() {
    expect(value(sys::ipv4_address("10.0.0.1")) < value(sys::ipv4_address("10.0.0.2")));
    expect(value(sys::ipv4_address("10.0.0.2")) >= value(sys::ipv4_address("10.0.0.1")));
}

void test_ipv6_address_identities() {
    expect(value(sys::ipv6_address{}) == value(sys::ipv6_address{"::"}));
    expect(value(sys::ipv6_address(0,0,0,0,0,0,0,0)) ==
           value(sys::ipv6_address{"0:0:0:0:0:0:0:0"}));
}

void test_ipv6_address_ordering() {
    expect(value(sys::ipv6_address("10::1")) < value(sys::ipv6_address("10::2")));
    expect(value(sys::ipv6_address("10::2")) >= value(sys::ipv6_address("10::1")));
}

template <class T>
void do_test_ip_address_properties() {
    using namespace sys::test;
    using rep = typename T::rep_type;
    falsify(
        [] (const Argument_array<2>& params) {
            T a{rep(params[0])};
            T b{rep(params[1])};
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<rep>(),
        make_parameter<rep>());
    falsify(
        [] (const Argument_array<1>& params) {
            T a{rep(params[0])};
            test::io_operators(a);
        },
        make_parameter<rep>());
    falsify(
        [] (const Argument_array<1>& params) {
            T a{rep(params[0])};
            test::bstream_insert_extract(a);
        },
        make_parameter<rep>());
}

void test_ip_address_properties() {
    do_test_ip_address_properties<sys::ipv4_address>();
    do_test_ip_address_properties<sys::ipv6_address>();
}
