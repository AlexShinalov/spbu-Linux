/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

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

#include <unistdx/net/netlink_poller>
#include <unistdx/net/netlink_socket>
#include <unistdx/net/network_interface>

sys::network_interface::network_interface(int index) {
    this->_name.resize(max_name_size());
    network_interface_request req{};
    req.ifr_ifindex = index;
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_get_name, req);
    constexpr auto n = sizeof(req.ifr_name)-1;
    if (req.ifr_name[n] != 0) { req.ifr_name[n] = 0; }
    this->_name = req.ifr_name;
    this->_name.resize(traits_type::length(this->_name.data()));
}

void sys::network_interface::flags(flag f) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    req.ifr_flags = static_cast<short>(f);
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_set_flags, req);
}

auto sys::network_interface::flags() const -> flag {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_get_flags, req);
    return static_cast<flag>(req.ifr_flags);
}

void sys::network_interface::setf(flag f) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_get_flags, req);
    flags(static_cast<flag>(req.ifr_flags) | f);
}

void sys::network_interface::unsetf(flag f) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_get_flags, req);
    flags(static_cast<flag>(req.ifr_flags) & (~f));
}

int sys::network_interface::index() const {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_get_index, req);
    return req.ifr_ifindex;
}

auto sys::network_interface::address() const -> interface_address_type {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_get_address, req);
    ipv4_socket_address addr(req.ifr_addr);
    s.call(fildes::operation::interface_get_network_mask, req);
    ipv4_socket_address netmask(req.ifr_netmask);
    return interface_address_type(addr.address(), netmask.address());
}

void sys::network_interface::address(const interface_address_type& addr) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    req.ifr_addr = *ipv4_socket_address{addr.address(),0}.get();
    socket s{socket_address_family::inet,socket_type::datagram};
    s.call(fildes::operation::interface_set_address, req);
    req.ifr_netmask = *ipv4_socket_address{addr.netmask(),0}.get();
    s.call(fildes::operation::interface_set_network_mask, req);
}

void
sys::network_interface::set_namespace(fd_type ns) {
    sys::ifinfo_request request;
    using f = sys::netlink_message_flags;
    request.header().type(sys::ifinfo_message_type::new_link);
    request.header().flags(f::request | f::acknowledge);
    request.message().family(sys::socket_address_family::netlink);
    using a = sys::ifinfo_attribute;
    request.add(a::network_namespace_fd, &ns, sizeof(ns));
    request.add(a::interface_name, this->_name);
    sys::netlink_socket sock(sys::netlink_protocol::route);
    request.write(sock);
    sys::ifinfo_message_container response;
    response.read(sock);
    // TODO
    /*
    for (auto& hdr : response) {
        auto m = hdr.sys::netlink_header::message<nlmsgerr>();
        if (hdr.error() && m->error != 0) {
            errno = -m->error;
            throw bad_call();
        }
    }
    */
}
