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

#include <mutex>

#include <unistdx/net/netlink_poller>
#include <unistdx/net/netlink_socket>
#include <unistdx/net/netlink_socket_address>
#include <unistdx/net/network_interface>
#include <unistdx/net/veth_interface>
#include <unistdx/test/language>
#include <unistdx/test/operator>
#include <unistdx/test/print_flags>

using namespace sys::test::lang;

void test_netlink_poller_first() {
    sys::netlink_socket_address endp(0, RTMGRP_IPV4_IFADDR);
    sys::netlink_socket sock(sys::netlink_protocol::route);
    sock.bind(endp);
    sys::event_poller poller;
    poller.insert({sock.fd(), sys::event::in});
    std::mutex mtx;
    poller.wait_for(mtx, std::chrono::milliseconds(1));
    for (const sys::epoll_event& ev : poller) {
        std::clog << "ev=" << ev << std::endl;
        if (ev.fd() == sock.fd()) {
            sys::ifaddr_message_container response;
            response.read(sock);
            for (sys::ifaddr_message_header& hdr : response) {
                std::string action;
                if (hdr.new_address()) {
                    action = "add";
                } else if (hdr.delete_address()) {
                    action = "del";
                }
                if (hdr.new_address() || hdr.delete_address()) {
                    sys::ifaddr_message* m = hdr.message();
                    sys::ipv4_address address;
                    for (auto& attr : m->attributes(response.length())) {
                        if (attr.type() == sys::ifaddr_attribute::address) {
                            address = *attr.data<sys::ipv4_address>();
                        }
                    }
                    std::clog << action << ' ' << address << std::endl;
                }
            }
        }
    }
}

void test_netlink_get_address() {
    sys::socket sock(
        sys::socket_address_family::netlink,
        sys::socket_type::raw,
        NETLINK_ROUTE
    );
    sock.unsetf(sys::open_flag::non_blocking);
    union {
        struct {
            sys::ifaddr_message_header hdr;
            sys::ifaddr_message payload;
        };
        char bytes[NLMSG_LENGTH(sizeof(sys::ifaddr_message))];
    } req{};
    req.hdr.flags(
        sys::netlink_message_flags::request |
        sys::netlink_message_flags::dump
    );
    req.hdr.type(sys::ifaddr_message_type::get_address);
    req.hdr.length(sizeof(req));
    req.payload.family(sys::socket_address_family::inet);
    ssize_t n = sock.send(&req, sizeof(req));
    std::clog << "n=" << n << std::endl;
    std::clog << "sizeof(req)=" << sizeof(req) << std::endl;
    sys::ifaddr_message_container response;
    response.read(sock);
    for (sys::ifaddr_message_header& hdr : response) {
        expect(!value(hdr.delete_address()));
        if (hdr.new_address()) {
            sys::ifaddr_message* m = hdr.message();
            sys::ipv4_address address;
            std::string name;
            for (auto& attr : m->attributes(response.length())) {
                if (attr.type() == sys::ifaddr_attribute::address) {
                    address = *attr.data<sys::ipv4_address>();
                } else if (attr.type() == sys::ifaddr_attribute::interface_name) {
                    name = attr.data<char>();
                }
            }
            std::clog
                << std::setw(3) << m->index() << ' '
                << std::setw(10) << name << ' '
                << address << '/' << m->prefix()
                << std::endl;
        }
    }
}

void test_ifaddr_attribute_print() {
    test::stream_insert_equals(
        "interface_address",
        sys::ifaddr_attribute::address
    );
    test::stream_insert_equals(
        "multicast_address",
        sys::ifaddr_attribute::multicast_address
    );
    test::stream_insert_equals(
        "anycast_address",
        sys::ifaddr_attribute::anycast_address
    );
    test::stream_insert_equals(
        "unknown",
        sys::ifaddr_attribute(1111)
    );
}

void test_ifaddr_attribute_test_print() {
    for (auto a : {sys::ifaddr_attribute::unspecified,
        sys::ifaddr_attribute::address,
        sys::ifaddr_attribute::local_address,
        sys::ifaddr_attribute::interface_name,
        sys::ifaddr_attribute::broadcast_address,
        sys::ifaddr_attribute::anycast_address,
        sys::ifaddr_attribute::address_info,
        sys::ifaddr_attribute::multicast_address,
        sys::ifaddr_attribute::flags,
    }) {
        expect(value("unknown") != value(test::stream_insert(a)));
    }
}
