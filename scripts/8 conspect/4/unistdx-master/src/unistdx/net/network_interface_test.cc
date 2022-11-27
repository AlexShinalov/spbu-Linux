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

#include <unistdx/ipc/process>
#include <unistdx/net/bridge_interface>
#include <unistdx/net/netlink_poller>
#include <unistdx/net/network_interface>
#include <unistdx/net/veth_interface>
#include <unistdx/test/config>
#include <unistdx/test/language>
#include <unistdx/test/print_flags>

using namespace sys::test::lang;

inline void unshare_network() {
    #if !defined(UNISTDX_TEST_HAVE_UNSHARE)
    std::_Exit(77);
    #endif
    using f = sys::unshare_flag;
    try {
        sys::this_process::unshare(f::network | f::users);
    } catch (const sys::bad_call& err) {
        if (err.errc() == std::errc::invalid_argument) {
            std::_Exit(77);
        }
    }
}

void test_network_interface_flags() {
    unshare_network();
    using f = sys::network_interface::flag;
    sys::network_interface lo("lo");
    //expect(value(f{}) == value((lo.flags() & f::up)));
    lo.flags(lo.flags() | f::up);
    expect(value(f::up) == value((lo.flags() & f::up)));
    lo.unsetf(f::up);
    expect(value(f{}) == value((lo.flags() & f::up)));
    lo.setf(f::up);
    expect(value(f::up) == value((lo.flags() & f::up)));
}

void test_network_interface_index() {
    unshare_network();
    sys::network_interface lo("lo");
    auto index = lo.index();
    sys::network_interface lo2(index);
    expect(value(index) == value(lo2.index()));
}

void test_bridge_interface_add() {
    unshare_network();
    using f = sys::network_interface::flag;
    sys::bridge_interface br("br0");
    br.up();
    expect(value(f::up) == value((br.flags() & f::up)));
    br.down();
    expect(value(f{}) == value((br.flags() & f::up)));
}
