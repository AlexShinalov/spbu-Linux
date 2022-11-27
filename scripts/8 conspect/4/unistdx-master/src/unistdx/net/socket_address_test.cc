/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020, 2021 Ivan Gankevich

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

#include <unistdx/config>

#if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
#include <linux/rtnetlink.h>
#endif

#include <iostream>
#include <random>
#include <vector>

#include <unistdx/net/socket_address>

#include <unistdx/test/language>
#include <unistdx/test/operator>

using namespace sys::test::lang;

inline void
expect_unix_socket_address(const std::string& string_representation) {
    using namespace sys::test::lang;
    std::stringstream tmp;
    tmp << string_representation;
    sys::socket_address actual_value{};
    tmp >> actual_value;
    expect(value(sys::socket_address_family::unix) == value(actual_value.family()));
}

template <class T>
struct PRNG {
    using traits_type = sys::ipaddr_traits<T>;
    using int_type = typename T::rep_type;
    using sa = typename traits_type::socket_address_type;
    typedef std::independent_bits_engine<
        std::random_device,
        8*sizeof(int_type),
        int_type> engine_type;
    engine_type generator;

    inline sa random_address() { return sa{T{generator()}, 0}; }

};

template <class T>
void do_test_socket_address_write_read_multiple() {
    PRNG<T> prng;
    std::vector<sys::socket_address> addrs(10);
    std::generate(addrs.begin(), addrs.end(),
                  [&] () { return prng.random_address(); });
    // write
    std::stringstream os;
    std::ostream_iterator<sys::socket_address> oit(os, "\n");
    std::copy(addrs.begin(), addrs.end(), oit);
    // read
    std::vector<sys::socket_address> addrs2;
    std::istream_iterator<sys::socket_address> iit(os), eos;
    std::copy(iit, eos, std::back_inserter(addrs2));
    expect(value(addrs) == value(addrs2));
}

void test_socket_address_write_read_multiple() {
    do_test_socket_address_write_read_multiple<sys::ipv4_address>();
    do_test_socket_address_write_read_multiple<sys::ipv6_address>();
}

void test_socket_address_ipv4_all() {
    expect(value(sizeof(sys::sockinet4_type)) ==
           value(sys::socket_address(sys::ipv4_socket_address{{127,0,0,1}, 0}).size()));
    // basic functionality
    test::stream_extract("0.0.0.0:0", sys::socket_address(sys::ipv4_socket_address{{0,0,0,0}, 0}));
    test::stream_extract("0.0.0.0:1234", sys::socket_address(sys::ipv4_socket_address{{0,0,0,0}, 1234}));
    test::stream_extract("0.0.0.0:65535", sys::socket_address(sys::ipv4_socket_address{{0,0,0,0}, 65535}));
    test::stream_extract("10.0.0.1:0", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 0}));
    test::stream_extract("255.0.0.1:0", sys::socket_address(sys::ipv4_socket_address{{255,0,0,1}, 0}));
    test::stream_extract(
        "255.255.255.255:65535",
        sys::socket_address(sys::ipv4_socket_address{{255,255,255,255}, 65535})
    );
    // out of range ports
    expect_unix_socket_address("0.0.0.0:65536");
    expect_unix_socket_address("0.0.0.1:65536");
    expect_unix_socket_address("10.0.0.1:100000");
    // out of range addrs
    expect_unix_socket_address("1000.0.0.1:0");
    // good spaces
    test::stream_extract(" 10.0.0.1:100", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 100}));
    test::stream_extract("10.0.0.1:100 ", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 100}));
    test::stream_extract(" 10.0.0.1:100 ", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 100}));
    // bad spaces
    expect_unix_socket_address("10.0.0.1: 100");
    expect_unix_socket_address("10.0.0.1 :100");
    expect_unix_socket_address("10.0.0.1 : 100");
    expect_unix_socket_address(" 10.0.0.1 : 100 ");
    // fancy addrs
    expect_unix_socket_address("10:100");
    expect_unix_socket_address("10.1:100");
    expect_unix_socket_address("10.0.1:100");
    test::stream_extract("", sys::socket_address());
    expect_unix_socket_address("anc:100");
    expect_unix_socket_address(":100");
    expect_unix_socket_address("10.0.0.0.1:100");
}

void test_socket_address_ipv6_all() {
    expect(value(sizeof(sys::sockinet6_type)) ==
           value(sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1},0}).size()));
    expect(value(sys::socket_address(sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1},1234}),100)) ==
           value(sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1}, 100})));
    // basic functionality
    test::stream_extract("[::1]:0", sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1}, 0}));
    test::stream_extract("[1::1]:0", sys::socket_address(sys::ipv6_socket_address{{1,0,0,0,0,0,0,1}, 0}));
    test::stream_extract("[::]:0", sys::socket_address(sys::ipv6_socket_address{{0,0,0,0,0,0,0,0}, 0}));
    test::stream_extract(
        "[2001:1:0::123]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x2001,1,0,0,0,0,0,0x123}, 0})
    );
    test::stream_extract(
        "[0:0:0:0:0:0:0:0]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,0}, 0})
    );
    test::stream_extract(
        "[0:0:0:0:0:0:0:0]:1234",
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,0}, 1234})
    );
    test::stream_extract(
        "[0:0:0:0:0:0:0:0]:65535",
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,0}, 65535})
    );
    test::stream_extract(
        "[10:1:0:1:0:0:0:0]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x10,1,0,1,0,0,0,0}, 0})
    );
    test::stream_extract(
        "[255:0:0:1:1:2:3:4]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x255,0,0,1,1,2,3,4}, 0})
    );
    test::stream_extract(
        "[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535",
        sys::socket_address(sys::ipv6_socket_address{{0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff}, 65535})
    );
    // out of range ports
    expect_unix_socket_address("[::1]:65536");
    expect_unix_socket_address("[::0]:65536");
    expect_unix_socket_address("[::0]:100000");
    // out of range addrs
    expect_unix_socket_address("[1ffff::1]:0");
    // good spaces
    test::stream_extract(" [10::1]:100", sys::socket_address(sys::ipv6_socket_address{{0x10,0,0,0,0,0,0,1}, 100}));
    test::stream_extract("[10::1]:100 ", sys::socket_address(sys::ipv6_socket_address{{0x10,0,0,0,0,0,0,1}, 100}));
    test::stream_extract(" [10::1]:100 ", sys::socket_address(sys::ipv6_socket_address{{0x10,0,0,0,0,0,0,1}, 100}));
    // bad spaces
    expect_unix_socket_address("[10::1]: 100");
    expect_unix_socket_address("[10::1 ]:100");
    expect_unix_socket_address("[10::1 ]: 100");
    expect_unix_socket_address(" [10::1 ]: 100 ");
    // fancy addrs
    expect_unix_socket_address("[::1::1]:0");
    expect_unix_socket_address("[:::]:0");
    expect_unix_socket_address("[:]:0");
    expect_unix_socket_address("[]:0");
    expect_unix_socket_address("]:0");
    expect_unix_socket_address("[:0");
    expect_unix_socket_address("[10:0:0:0:0:0:0:0:1]:0");
    // IPv4 mapped addrs
    test::stream_extract(
        "[::ffff:127.1.2.3]:0",
        sys::socket_address(sys::ipv6_socket_address{{0,0,0,0xffff,0x127,1,2,3}, 0})
    );
}

void test_socket_address_operator_bool() {
    // operator bool
    expect(!bool(sys::socket_address()));
    expect(!sys::socket_address());
    // operator bool (IPv4)
    expect(!bool(sys::socket_address("0.0.0.0:0")));
    expect(!sys::socket_address("0.0.0.0:0"));
    expect(bool(sys::socket_address("127.0.0.1:100")));
    expect(!value(!sys::socket_address("127.0.0.1:100")));
    expect(bool(sys::socket_address("127.0.0.1:0")));
    expect(!value(!sys::socket_address("127.0.0.1:0")));
    // operator bool (IPv6)
    expect(!bool(sys::socket_address("[0:0:0:0:0:0:0:0]:0")));
    expect(!sys::socket_address("[0:0:0:0:0:0:0:0]:0"));
    expect(!bool(sys::socket_address("[::]:0")));
    expect(!sys::socket_address("[::]:0"));
    expect(bool(sys::socket_address("[::1]:0")));
    expect(!value(!sys::socket_address("[::1]:0")));
}

void test_socket_address_operator_copy() {
    expect(value(sys::socket_address(sys::socket_address("10.0.0.1:1234"),100)) ==
           value(sys::socket_address("10.0.0.1:100")));
}

void test_socket_address_literals() {
    using sys::ipv4_address;
    using sys::ipv6_address;
    constexpr ipv4_address any4;
    constexpr ipv6_address any6;
    sys::socket_address endpU(sys::ipv6_socket_address(ipv6_address(), 1234), 100);
    sys::socket_address endpV(sys::ipv6_socket_address(ipv6_address(), 100));
    expect(value(endpU) == value(endpV));
}

void test_socket_address_unix_domain() {
    expect(value(sys::socket_address_family::unix) ==
           value(sys::socket_address("/path").family()));
    std::clog << sys::socket_address("/path/to/socket") << std::endl;
    std::clog << sys::socket_address("\0/path/to/socket") << std::endl;
    expect(value(sys::socket_address("/path")) == value(sys::socket_address("/path")));
    expect(value(sys::socket_address("\0/path")) == value(sys::socket_address("\0/path")));
    expect(value(sys::socket_address("\0/path")) != value(sys::socket_address("/path")));
    expect(value(sys::socket_address("/path")) != value(sys::socket_address("\0/path")));
    expect(value(sys::socket_address("/path")) != value(sys::socket_address("\0/path")));
    expect(!(value(sys::socket_address("/path")) <
             value(sys::socket_address("/path"))));
    expect(value(bool(sys::socket_address("/path"))));
    expect(value(bool(sys::socket_address("\0/path"))));
    expect(value(bool(sys::socket_address("\0"))));
    expect(!value(sys::socket_address()));
}

#if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
void test_socket_address_netlink() {
    expect(value(sys::socket_address_family::netlink) ==
           value(sys::netlink_socket_address(RTMGRP_IPV4_IFADDR).family()));
    expect(value(sys::netlink_socket_address(RTMGRP_IPV4_IFADDR)) ==
           value(sys::netlink_socket_address(RTMGRP_IPV4_IFADDR)));
    expect(!(value(sys::netlink_socket_address(RTMGRP_IPV4_IFADDR)) <
             value(sys::netlink_socket_address(RTMGRP_IPV4_IFADDR))));
    expect(value(sys::netlink_socket_address()) !=
           value(sys::netlink_socket_address(RTMGRP_IPV4_IFADDR)));
}
#endif

void test_socket_address_bad_family() {
    sys::socket_address a;
    a.get()->sa_family = 1111;
    sys::socket_address b;
    b.get()->sa_family = 1111;
    expect(!(value(a) < value(b)));
    expect(a != b);
    expect(value(0u) == value(a.size()));
    expect(value(0u) == value(b.size()));
}

void test_ipv4_socket_address_port() {
    using t = sys::ipaddr_traits<sys::ipv4_address>;
    sys::ipv4_socket_address sa{{127,0,0,1},1234};
    expect(value(1234u) == value(sa.port()));
    expect(value(1234u) == value(t::port(sa)));
}

void test_ipv6_socket_address_port() {
    using t = sys::ipaddr_traits<sys::ipv6_address>;
    sys::ipv6_socket_address sa{{0,0,0,0,0,0,0,0},1234};
    expect(value(1234u) == value(sa.port()));
    expect(value(1234u) == value(t::port(sa)));
}

template <class Address, class T>
void do_test_socket_address_properties() {
    using namespace sys::test;
    using rep = typename T::rep_type;
    falsify(
        [] (const Argument_array<4>& params) {
            sys::socket_address a{Address{T{rep(params[0])}, sys::port_type(params[1])}};
            sys::socket_address b{Address{T{rep(params[2])}, sys::port_type(params[3])}};
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<rep>(),
        make_parameter<sys::port_type>(),
        make_parameter<rep>(),
        make_parameter<sys::port_type>());
    falsify(
        [] (const Argument_array<2>& params) {
            sys::socket_address a{Address{T{rep(params[0])}, sys::port_type(params[1])}};
            test::io_operators(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::port_type>());
    falsify(
        [] (const Argument_array<2>& params) {
            sys::socket_address a{Address{T{rep(params[0])}, sys::port_type(params[1])}};
            test::bstream_insert_extract(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::port_type>());
}

void test_socket_address_properties() {
    do_test_socket_address_properties<sys::ipv4_socket_address,sys::ipv4_address>();
    do_test_socket_address_properties<sys::ipv6_socket_address,sys::ipv6_address>();
}

template <class Address, class T>
void do_test_ip_socket_address_properties() {
    using namespace sys::test;
    using rep = typename T::rep_type;
    falsify(
        [] (const Argument_array<4>& params) {
            Address a{T{rep(params[0])}, sys::port_type(params[1])};
            Address b{T{rep(params[2])}, sys::port_type(params[3])};
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<rep>(),
        make_parameter<sys::port_type>(),
        make_parameter<rep>(),
        make_parameter<sys::port_type>());
    falsify(
        [] (const Argument_array<2>& params) {
            Address a{T{rep(params[0])}, sys::port_type(params[1])};
            test::io_operators(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::port_type>());
    falsify(
        [] (const Argument_array<2>& params) {
            Address a{T{rep(params[0])}, sys::port_type(params[1])};
            test::bstream_insert_extract(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::port_type>());
}

void test_ip_socket_address_properties() {
    do_test_ip_socket_address_properties<sys::ipv4_socket_address,sys::ipv4_address>();
    do_test_ip_socket_address_properties<sys::ipv6_socket_address,sys::ipv6_address>();
}

void test_unix_socket_address_properties() {
    using namespace sys::test;
    auto prng = current_test->prng();
    falsify(
        [&prng] (const Argument_array<2>& params) {
            std::uniform_int_distribution<char> dist('a','z');
            std::string name;
            auto size = params[0];
            name.reserve(size);
            for (size_t i=0; i<size; ++i) { name += dist(prng); }
            sys::unix_socket_address a(name.data());
            name.clear();
            size = params[1];
            name.reserve(size);
            for (size_t i=0; i<size; ++i) { name += dist(prng); }
            sys::unix_socket_address b(name.data());
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<size_t>(0, sys::unix_socket_address::max_length()),
        make_parameter<size_t>(0, sys::unix_socket_address::max_length()));
    falsify(
        [&prng] (const Argument_array<1>& params) {
            auto size = params[0];
            std::string name;
            name.reserve(size);
            std::uniform_int_distribution<char> dist('a','z');
            for (size_t i=0; i<size; ++i) { name += dist(prng); }
            sys::unix_socket_address a(name.data());
            test::io_operators(a);
        },
        make_parameter<size_t>(0, sys::unix_socket_address::max_length()));
    falsify(
        [&prng] (const Argument_array<1>& params) {
            auto size = params[0];
            std::string name;
            name.reserve(size);
            std::uniform_int_distribution<char> dist('a','z');
            for (size_t i=0; i<size; ++i) { name += dist(prng); }
            sys::unix_socket_address a(name.data());
            test::bstream_insert_extract(a);
        },
        make_parameter<size_t>(0, sys::unix_socket_address::max_length()));
}

void test_netlink_socket_address_properties() {
    using namespace sys::test;
    falsify(
        [] (const Argument_array<2>& params) {
            sys::netlink_socket_address a{sys::pid_type(params[0])};
            sys::netlink_socket_address b{sys::pid_type(params[1])};
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<sys::pid_type>(),
        make_parameter<sys::pid_type>());
    falsify(
        [] (const Argument_array<1>& params) {
            sys::netlink_socket_address a{sys::pid_type(params[0])};
            test::io_operators(a);
        },
        make_parameter<sys::pid_type>());
    falsify(
        [] (const Argument_array<1>& params) {
            sys::netlink_socket_address a{sys::pid_type(params[0])};
            test::bstream_insert_extract(a);
        },
        make_parameter<sys::pid_type>());
}
