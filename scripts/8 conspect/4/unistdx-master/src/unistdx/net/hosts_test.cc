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
#include <unistdx/test/language>

void test_hosts() {
    using namespace sys::test::lang;
    using f = sys::host_flags;
    sys::hosts hosts;
    hosts.socket_type(sys::socket_type::stream);
    hosts.flags(f::canonical_name | f::all | f::v4_mapped);
    hosts.update("very-long-host-name-that-does-not-exist");
    expect(value(0) == value(std::distance(hosts.begin(),hosts.end())));
    /*
    hosts.update("m1.cmms", "ssh");
    std::clog << "num-addresses " << std::distance(hosts.begin(),hosts.end()) << std::endl;
    for (const auto& host : hosts) {
        if (host.canonical_name()) {
            std::clog << "host.canonical_name()=" << host.canonical_name() << std::endl;
        }
        std::clog << "host.family()=" << host.family() << std::endl;
        std::clog << "host.socket_type()=" << int(host.socket_type()) << std::endl;
        std::clog << "host.socket_address_size()=" << host.socket_address_size() << std::endl;
        std::clog << "host.socket_address()=" << host.socket_address() << std::endl;
        std::clog << "sys::host_name(host.socket_address())=" << sys::host_name(host.socket_address()) << std::endl;
    }
    std::clog << "sys::service_name(22)=" << sys::service_name(22) << std::endl;
    */
}
