/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020, 2021 Ivan Gankevich

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

#include <cmath>
#include <random>
#include <sstream>

#include <unistdx/config>
#include <unistdx/net/interface_address>
#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/operator>
#include <unistdx/test/properties>

using namespace sys::test::lang;

void test_interface_address_localhost_ipv6() {
    typedef sys::interface_address<sys::ipv6_address> ifaddr_type;
    typedef sys::ipaddr_traits<sys::ipv6_address> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    std::stringstream str;
    str << ifa;
    expect(value(str.str()) == value("0:0:0:0:0:0:0:1/128"));
}

template <class T>
void test_interface_address_input_output_operators_localhost() {
    typedef sys::interface_address<T> ifaddr_type;
    typedef sys::ipaddr_traits<T> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    test::io_operators(ifa);
}

void test_interface_address_input_output_operators_localhost() {
    test_interface_address_input_output_operators_localhost<sys::ipv4_address>();
    test_interface_address_input_output_operators_localhost<sys::ipv6_address>();
}

template <class T>
void do_test_interface_address_properties() {
    using namespace sys::test;
    using rep = typename T::rep_type;
    falsify(
        [] (const Argument_array<4>& params) {
            sys::interface_address<T> a{T{rep(params[0])}, sys::prefix_type(params[1])};
            sys::interface_address<T> b{T{rep(params[2])}, sys::prefix_type(params[3])};
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8),
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8));
    falsify(
        [] (const Argument_array<2>& params) {
            sys::interface_address<T> a{T{rep(params[0])}, sys::prefix_type(params[1])};
            test::io_operators(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8));
    falsify(
        [] (const Argument_array<2>& params) {
            sys::interface_address<T> a{T{rep(params[0])}, sys::prefix_type(params[1])};
            test::bstream_insert_extract(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8));
    falsify(
        [] (const Argument_array<1>& params) {
            using t = sys::ipaddr_traits<T>;
            expect(sys::interface_address<T>(T{rep(params[0])}, t::widearea_mask()).is_widearea());
        },
        make_parameter<rep>());
}

void test_interface_address_properties() {
    do_test_interface_address_properties<sys::ipv4_address>();
    do_test_interface_address_properties<sys::ipv6_address>();
}

template <class T>
void test_interface_address_loopback() {
    using ifaddr_type = sys::interface_address<T>;
    using t = sys::ipaddr_traits<T>;
    expect(ifaddr_type(t::localhost(), t::loopback_mask()).is_loopback());
}

void test_interface_address_loopback() {
    test_interface_address_loopback<sys::ipv4_address>();
    test_interface_address_loopback<sys::ipv6_address>();
}

void test_interface_address_contains_ipv4() {
    typedef sys::interface_address<sys::ipv4_address> ifaddr_type;
    typedef sys::ipaddr_traits<sys::ipv4_address> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    expect(ifa.contains({127,0,0,1}));
    expect(ifa.contains({127,0,0,2}));
    expect(ifa.contains(*ifa.begin()));
    expect(!ifa.contains(*ifa.end()));
    expect(!ifa.contains({127,0,0,0}));
    expect(!ifa.contains(*--ifa.begin()));
}

void test_interface_address_count_ipv4() {
    typedef sys::interface_address<sys::ipv4_address> ifaddr_type;
    typedef sys::ipaddr_traits<sys::ipv4_address> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    expect(value((1u<<24)-2u) == value(ifa.count()));
}
