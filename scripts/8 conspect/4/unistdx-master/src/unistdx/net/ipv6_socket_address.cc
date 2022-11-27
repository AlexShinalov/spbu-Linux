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
#include <unistdx/net/ipv6_socket_address>

namespace {

    using Left_br = sys::bits::Const_char<'['>;
    using Right_br = sys::bits::Const_char<']'>;

    inline void
    init_addr(sys::sockinet6_type& addr6, const sys::ipv6_address& a, sys::port_type p) {
        using namespace sys;
        addr6.sin6_family = static_cast<sa_family_type>(socket_address_family::ipv6);
        addr6.sin6_addr = a;
        addr6.sin6_port = to_network_format<port_type>(p);
    }

}

std::ostream&
sys::operator<<(std::ostream& out, const ipv6_socket_address& rhs) {
    using bits::Colon;
    return out << Left_br() << rhs.address() << Right_br() << Colon() << rhs.port();
}

std::istream&
sys::operator>>(std::istream& in, ipv6_socket_address& rhs) {
    using bits::Colon;
    std::istream::sentry s(in);
    if (s) {
        port_type port = 0;
        ios_guard g(in);
        in.unsetf(std::ios_base::skipws);
        ipv6_address host6;
        if (in >> Left_br() >> host6 >> Right_br() >> Colon() >> port) {
            init_addr(rhs._address, host6, port);
        }
    }
    return in;
}

sys::bstream&
sys::operator<<(bstream& out, const ipv6_socket_address& rhs) {
    out << rhs.address();
    out << port_type(rhs._address.sin6_port);
    return out;
}

sys::bstream&
sys::operator>>(bstream& in, ipv6_socket_address& rhs) {
    rhs._address.sin6_family = sa_family_type(socket_address_family::ipv6);
    port_type port{};
    ipv6_address addr;
    in >> addr >> port;
    rhs._address.sin6_addr = addr;
    rhs._address.sin6_port = port;
    return in;
}

sys::ipv6_socket_address::ipv6_socket_address(const char* host, port_type port) {
    ipv6_address a6;
    std::stringstream tmp(host);
    if (tmp >> a6) {
        init_addr(this->_address, a6, port);
    }
}
