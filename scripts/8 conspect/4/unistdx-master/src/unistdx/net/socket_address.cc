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

#include <unistdx/net/socket_address>

#include <cstring>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>

#include <unistdx/base/ios_guard>
#include <unistdx/bits/addr_parse>

std::ostream&
sys::operator<<(std::ostream& out, const socket_address& rhs) {
    switch (rhs.family()) {
        case socket_address_family::ipv6: out << rhs.ref<ipv6_socket_address>(); break;
        case socket_address_family::ipv4: out << rhs.ref<ipv4_socket_address>(); break;
        case socket_address_family::unix: out << rhs.ref<unix_socket_address>(); break;
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        case socket_address_family::netlink: out << rhs.ref<netlink_socket_address>(); break;
        #endif
        default: break;
    }
    return out;
}

std::istream&
sys::operator>>(std::istream& in, socket_address& rhs) {
    using bits::Colon;
    std::istream::sentry s(in);
    if (s) {
        ipv4_socket_address ipv4;
        ios_guard g(in);
        in.unsetf(std::ios_base::skipws);
        std::streampos oldg = in.tellg();
        if (in >> ipv4) {
            rhs.ref<ipv4_socket_address>() = ipv4;
        } else {
            in.clear();
            in.seekg(oldg);
            ipv6_socket_address ipv6;
            if (in >> ipv6) {
                rhs.ref<ipv6_socket_address>() = ipv6;
            } else {
                in.clear();
                in.seekg(oldg);
                unix_socket_address unix;
                if (in >> unix) {
                    rhs.ref<unix_socket_address>() = unix;
                } else {
                    in.clear();
                    in.seekg(oldg);
                    netlink_socket_address netlink;
                    if (in >> netlink) {
                        rhs.ref<netlink_socket_address>() = netlink;
                    } else {
                        rhs.ref<socket_address_family>() = socket_address_family{};
                    }
                }
            }
        }
    }
    return in;
}

sys::bstream&
sys::operator<<(bstream& out, const socket_address& rhs) {
    out << rhs.family();
    switch (rhs.family()) {
        case socket_address_family::ipv6: out << rhs.ref<ipv6_socket_address>(); break;
        case socket_address_family::ipv4: out << rhs.ref<ipv4_socket_address>(); break;
        case socket_address_family::unix: out << rhs.ref<unix_socket_address>(); break;
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        case socket_address_family::netlink: out << rhs.ref<netlink_socket_address>(); break;
        #endif
        default: break;
    }
    return out;
}

sys::bstream&
sys::operator>>(bstream& in, socket_address& rhs) {
    socket_address_family fam;
    in >> fam;
    rhs.ref<socket_address_family>() = fam;
    switch (rhs.family()) {
        case socket_address_family::ipv6: in >> rhs.ref<ipv6_socket_address>(); break;
        case socket_address_family::ipv4: in >> rhs.ref<ipv4_socket_address>(); break;
        case socket_address_family::unix: in >> rhs.ref<unix_socket_address>(); break;
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        case socket_address_family::netlink: in >> rhs.ref<netlink_socket_address>(); break;
        #endif
        default: break;
    }
    return in;
}

sys::socket_length_type
sys::socket_address::size() const noexcept {
    switch (this->family()) {
        case socket_address_family::ipv4: return ipv4_socket_address::size();
        case socket_address_family::ipv6: return ipv6_socket_address::size();
        case socket_address_family::unix: return ref<unix_socket_address>().size();
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        case socket_address_family::netlink: return netlink_socket_address::size();
        #endif
        default: return 0;
    }
}

bool
sys::socket_address::operator<(const socket_address& rhs) const noexcept {
    if (this->family() == rhs.family()) {
        switch (this->family()) {
            case socket_address_family::unix:
                return ref<unix_socket_address>() < rhs.ref<unix_socket_address>();
            case socket_address_family::ipv4:
                return ref<ipv4_socket_address>() < rhs.ref<ipv4_socket_address>();
            case socket_address_family::ipv6:
                return ref<ipv6_socket_address>() < rhs.ref<ipv6_socket_address>();
            #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
            case socket_address_family::netlink:
                return ref<netlink_socket_address>() < rhs.ref<netlink_socket_address>();
            #endif
            default:
                return false;
        }
    } else {
        return this->sa_family() < rhs.sa_family();
    }
}

bool
sys::socket_address::operator==(const socket_address& rhs) const noexcept {
    if (this->family() != rhs.family() && this->sa_family() &&
        rhs.sa_family()) {
        return false;
    }
    switch (this->family()) {
        case socket_address_family::unix:
            return ref<unix_socket_address>() == rhs.ref<unix_socket_address>();
        case socket_address_family::ipv4:
            return ref<ipv4_socket_address>() == rhs.ref<ipv4_socket_address>();
        case socket_address_family::ipv6:
            return ref<ipv6_socket_address>() == rhs.ref<ipv6_socket_address>();
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        case socket_address_family::netlink:
            return ref<netlink_socket_address>() == rhs.ref<netlink_socket_address>();
        #endif
        default:
            return this->sa_family() == 0 && rhs.sa_family() == 0;
    }
}

sys::socket_address::operator bool() const noexcept {
    switch (this->family()) {
        case socket_address_family::unix: return bool(ref<unix_socket_address>());
        case socket_address_family::ipv4: return bool(ref<ipv4_socket_address>());
        case socket_address_family::ipv6: return bool(ref<ipv6_socket_address>());
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        case socket_address_family::netlink: return bool(ref<netlink_socket_address>());
        #endif
        default: return false;
    }
}

sys::socket_address::socket_address(const char* s) noexcept {
    if (*s == 0) {
        ref<unix_socket_address>() = unix_socket_address(s);
    } else {
        std::stringstream tmp(s);
        tmp >> *this;
    }
}

size_t std::hash<sys::socket_address>::operator()(const sys::socket_address& rhs) const noexcept {
    using namespace sys;
    switch (rhs.family()) {
        case socket_address_family::unix:
            return hash<unix_socket_address>()(rhs.ref<unix_socket_address>());
        case socket_address_family::ipv4:
            return hash<ipv4_socket_address>()(rhs.ref<ipv4_socket_address>());
        case socket_address_family::ipv6:
            return hash<ipv6_socket_address>()(rhs.ref<ipv6_socket_address>());
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        case socket_address_family::netlink:
            return hash<netlink_socket_address>()(rhs.ref<netlink_socket_address>());
        #endif
        default: return 0;
    }
}
