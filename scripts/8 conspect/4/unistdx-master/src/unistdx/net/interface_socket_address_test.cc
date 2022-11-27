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

#include <unistdx/net/interface_socket_address>
#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/language>
#include <unistdx/test/operator>

void test_interface_socket_address() {
    using namespace sys::test::lang;
    sys::interface_socket_address<sys::ipv4_address> isa{{127,0,0,1},24,33333};
    expect(value(sys::ipv4_address{127,0,0,1}) == value(isa.address()));
    expect(value(33333) == value(isa.port()));
    expect(
        value(sys::socket_address{sys::ipv4_socket_address{{127,0,0,1},33333}})
        ==
        value(isa.socket_address())
    );
    test::bstream_insert_extract(isa);
    test::stream_insert_equals("127.0.0.1/24:33333", isa);
    test::io_operators(isa);
}

template <class T>
void do_test_interface_socket_address_properties() {
    using namespace sys::test;
    using rep = typename T::rep_type;
    falsify(
        [] (const Argument_array<6>& params) {
            sys::interface_socket_address<T> a{
                T{rep(params[0])},
                sys::prefix_type(params[1]),
                sys::port_type(params[2])
            };
            sys::interface_socket_address<T> b{
                T{rep(params[2])},
                sys::prefix_type(params[3]),
                sys::port_type(params[4])
            };
            test::equality_and_hash(a, b);
            test::equality_and_hash(a, a);
            test::equality_and_hash(b, b);
        },
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8),
        make_parameter<sys::port_type>(),
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8),
        make_parameter<sys::port_type>());
    falsify(
        [] (const Argument_array<3>& params) {
            sys::interface_socket_address<T> a{
                T{rep(params[0])},
                sys::prefix_type(params[1]),
                sys::port_type(params[2])
            };
            test::io_operators(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8),
        make_parameter<sys::port_type>());
    falsify(
        [] (const Argument_array<3>& params) {
            sys::interface_socket_address<T> a{
                T{rep(params[0])},
                sys::prefix_type(params[1]),
                sys::port_type(params[2])
            };
            test::bstream_insert_extract(a);
        },
        make_parameter<rep>(),
        make_parameter<sys::prefix_type>(0, sizeof(rep)*8),
        make_parameter<sys::port_type>());
}

void test_interface_socket_address_properties() {
    do_test_interface_socket_address_properties<sys::ipv4_address>();
    do_test_interface_socket_address_properties<sys::ipv6_address>();
}
