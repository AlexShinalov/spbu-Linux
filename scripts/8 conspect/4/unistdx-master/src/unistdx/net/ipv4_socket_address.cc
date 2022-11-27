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

#include <sstream>

#include <unistdx/bits/addr_parse>
#include <unistdx/net/ipv4_socket_address>

namespace {

    inline void
    init_addr(sys::sockinet4_type& addr4, sys::ipv4_address a, sys::port_type p) {
        using namespace sys;
        addr4.sin_family = static_cast<sa_family_type>(socket_address_family::ipv4);
        addr4.sin_addr = a;
        addr4.sin_port = to_network_format<port_type>(p);
    }

}

std::ostream&
sys::operator<<(std::ostream& out, const ipv4_socket_address& rhs) {
    return out << rhs.address() << ':' << rhs.port();
}

std::istream&
sys::operator>>(std::istream& in, ipv4_socket_address& rhs) {
    using bits::Colon;
    std::istream::sentry s(in);
    if (s) {
        ipv4_address host;
        port_type port = 0;
        ios_guard g(in);
        in.unsetf(std::ios_base::skipws);
        if (in >> host >> Colon() >> port) {
            init_addr(rhs._address, host, port);
        }
    }
    return in;
}

sys::bstream&
sys::operator<<(bstream& out, const ipv4_socket_address& rhs) {
    out << rhs.address();
    out << port_type(rhs._address.sin_port);
    return out;
}

sys::bstream&
sys::operator>>(bstream& in, ipv4_socket_address& rhs) {
    rhs._address.sin_family = sa_family_type(socket_address_family::ipv4);
    port_type port{};
    ipv4_address addr;
    in >> addr >> port;
    rhs._address.sin_addr = addr;
    rhs._address.sin_port = port;
    return in;
}

sys::ipv4_socket_address::ipv4_socket_address(const char* host, port_type port) {
    ipv4_address a4;
    std::stringstream tmp(host);
    if (tmp >> a4) {
        init_addr(this->_address, a4, port);
    }
}
