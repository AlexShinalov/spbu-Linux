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

#include <vector>

#include <unistdx/base/log_message>
#include <unistdx/io/pipe>
#include <unistdx/ipc/process>
#include <unistdx/net/interface_addresses>
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

template <class ... Args>
inline void
log(const Args& ... args) {
    sys::log_message("tst", args...);
}

void print_network_interfaces() {
    for (const auto& ifa : sys::interface_addresses()) {
        //sys::socket_address addr(*ifa.ifa_addr);
        std::clog << std::setw(20) << ifa.ifa_name << std::endl;
    }
}

#if defined(UNISTDX_TEST_HAVE_UNSHARE)
void test_veth_up_down() {
    unshare_network();
    {
        sys::veth_interface veth0("veth0", "veth1");
        using flags = sys::network_interface::flag;
        veth0.up();
        expect(value(flags::up) == value((veth0.flags() & flags::up)));
        veth0.down();
        expect(value(flags{}) == value((veth0.flags() & flags::up)));
        sys::test::print_flags(veth0.flags());
    }
    {
        std::vector<sys::veth_interface> veths;
        veths.emplace_back("x0", "y0");
        veths.emplace_back("x1", "y1");
        veths.emplace_back("x2", "y2");
        veths.emplace_back("x3", "y3");
        for (auto& v : veths) {
            std::clog << std::setw(10) << v.index();
            std::clog << std::setw(20) << v.name();
            std::clog << std::setw(20) << v.peer().index();
            std::clog << std::setw(20) << v.peer().name();
            std::clog << std::endl;
        }
        veths.resize(100);
    }
    print_network_interfaces();
}
#endif

void test_bare() {
    unshare_network();
    std::vector<sys::veth_interface> veths;
    for (int i=0; i<10; ++i) {
        std::clog << "i=" << i << std::endl;
        veths.emplace_back("x" + std::to_string(i), "vx" + std::to_string(i));
    }
    for (auto& v : veths) {
        v.up();
        v.down();
        std::clog << "v.index()=" << v.index() << std::endl;
        std::clog << "v.peer().index()=" << v.peer().index() << std::endl;
        v.destroy();
    }
}

#if defined(UNISTDX_TEST_HAVE_UNSHARE)
/*
void test_unshare() {
    #if !defined(UNISTDX_TEST_HAVE_UNSHARE)
    std::exit(77);
    #endif
    using f = sys::unshare_flag;
    sys::this_process::unshare(f::users | f::network);
    test_bare();
}

void test_clone_unshare() {
    #if !defined(UNISTDX_TEST_HAVE_UNSHARE)
    std::exit(77);
    #endif
    sys::process child{[&] () {
        try {
            test_unshare();
            return 0;
        } catch (const std::exception& err) {
            log("error: _", err.what());
        }
        return 1;
    }, sys::process_flag::fork, 4096*10};
    expect(value(0) == value(child.wait().exit_code()));
}

void test_clone() {
    #if !defined(UNISTDX_TEST_HAVE_UNSHARE)
    std::exit(77);
    #endif
    using pf = sys::process_flag;
    sys::pipe pipe;
    pipe.in().unsetf(sys::open_flag::non_blocking);
    pipe.out().unsetf(sys::open_flag::non_blocking);
    sys::process child{[&] () {
        try {
            pipe.out().close();
            char ch;
            pipe.in().read(&ch, 1);
            test_bare();
            return 0;
        } catch (const std::exception& err) {
            std::clog << "err.what()=" << err.what() << std::endl;
        }
        return 1;
    }, pf::signal_parent | pf::unshare_users | pf::unshare_network, 4096*10};
    child.init_user_namespace();
    pipe.in().close();
    pipe.out().write("x", 1);
    expect(value(0) == value(child.wait().exit_code()));
}
*/
#endif
