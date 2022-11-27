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

#include <unistdx/net/family>

#include <limits>

#include <unistdx/base/types>

namespace {

    typedef sys::u8 raw_family_type;

    enum struct portable_family_type: raw_family_type {
        unspecified = 0,
        inet = 1,
        inet6 = 2,
        unix = 3
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        , netlink = 4
        #endif
    };

    inline portable_family_type
    map_family_type(sys::socket_address_family t) {
        using f = sys::socket_address_family;
        switch (t) {
            case f::unspecified: return portable_family_type::unspecified;
            case f::inet: return portable_family_type::inet;
            case f::inet6: return portable_family_type::inet6;
            case f::unix: return portable_family_type::unix;
            #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
            case f::netlink: return portable_family_type::netlink;
            #endif
            default: return portable_family_type(
                std::numeric_limits<sys::sa_family_type>::max()
            );
        }
    };

    inline sys::socket_address_family
    map_family_type(portable_family_type t) {
        using f = sys::socket_address_family;
        switch (t) {
            case portable_family_type::unspecified: return f::unspecified;
            case portable_family_type::inet: return f::inet;
            case portable_family_type::inet6: return f::inet6;
            case portable_family_type::unix: return f::unix;
            #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
            case portable_family_type::netlink: return f::netlink;
            #endif
            default: return f(
                std::numeric_limits<sys::sa_family_type>::max()
            );
        }
    };

}

sys::bstream&
sys::operator<<(bstream& out, socket_address_family rhs) {
    return out << static_cast<raw_family_type>(map_family_type(rhs));
}

sys::bstream&
sys::operator>>(bstream& in, socket_address_family& rhs) {
    raw_family_type raw;
    in >> raw;
    rhs = map_family_type(static_cast<portable_family_type>(raw));
    return in;
}

const char*
sys::to_string(socket_address_family rhs) noexcept {
    switch (rhs) {
    case socket_address_family::unspecified: return "unspecified";
    case socket_address_family::inet: return "inet";
    case socket_address_family::inet6: return "inet6";
    case socket_address_family::unix: return "unix";
    #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
    case socket_address_family::netlink: return "netlink";
    #endif
    default: return "unknown";
    }
}

std::ostream&
sys::operator<<(std::ostream& out, const socket_address_family& rhs) {
    return out << to_string(rhs);
}
