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

#include <unistdx/net/netlink_poller>

const char*
sys::to_string(ifaddr_attribute rhs) {
    switch (rhs) {
        case ifaddr_attribute::unspecified: return "unspecified";
        case ifaddr_attribute::address: return "interface_address";
        case ifaddr_attribute::local_address: return "local_address";
        case ifaddr_attribute::interface_name: return "interface_name";
        case ifaddr_attribute::broadcast_address: return "broadcast_address";
        case ifaddr_attribute::anycast_address: return "anycast_address";
        case ifaddr_attribute::address_info: return "address_info";
        case ifaddr_attribute::multicast_address: return "multicast_address";
        case ifaddr_attribute::flags: return "flags";
        default: return "unknown";
    }
}

std::ostream&
sys::operator<<(std::ostream& out, const ifaddr_attribute& rhs) {
    return out << to_string(rhs);
}
