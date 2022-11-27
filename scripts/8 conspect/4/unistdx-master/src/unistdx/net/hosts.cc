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

#include <unistdx/net/hosts>
#include <unistdx/net/ipv4_socket_address>

sys::host_error_category sys::host_category;

std::string
sys::host_error_category::message(int ev) const noexcept {
    auto s = ::gai_strerror(ev);
    return s ? std::string(s) : std::string("unknown");
}

sys::string sys::host_name(const socket_address& address, host_name_flags flags) {
    string name(NI_MAXHOST);
    int ret = 0;
    while (true) {
        ret = ::getnameinfo(address.get(), address.size(),
                            &name[0], name.capacity(),
                            nullptr, 0, int(flags));
        if (ret != EAI_OVERFLOW) { break; }
        name.capacity(name.capacity()*2);
    }
    UNISTDX_GAI_CHECK(ret);
    return name;
}

sys::string sys::service_name(port_type port, host_name_flags flags) {
    socket_address address(ipv4_socket_address{{0,0,0,0},port});
    string name(NI_MAXSERV);
    int ret = 0;
    while (true) {
        ret = ::getnameinfo(address.get(), address.size(),
                            nullptr, 0,
                            &name[0], name.capacity(), int(flags));
        if (ret != EAI_OVERFLOW) { break; }
        name.capacity(name.capacity()*2);
    }
    UNISTDX_GAI_CHECK(ret);
    return name;
}
