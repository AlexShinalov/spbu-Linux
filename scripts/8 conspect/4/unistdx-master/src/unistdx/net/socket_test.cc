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

#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>
#include <unistdx/net/ipv4_socket_address>
#include <unistdx/net/socket>
#include <unistdx/net/socket_address>

#include <unistdx/test/language>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_socket_get_credentials() {
    const char* path = "\0test_socket";
    sys::unix_socket_address e(path);
    sys::socket sock(sys::socket_address_family::unix);
    sock.set(sys::socket::options::reuse_address);
    sock.bind(e);
    sock.unsetf(sys::open_flag::non_blocking);
    sock.set(sys::socket::options::pass_credentials);
    sock.listen();
    sys::process child([&] () {
        sys::socket s(sys::socket_address_family::unix);
        s.unsetf(sys::open_flag::non_blocking);
        s.set(sys::socket::options::pass_credentials);
        s.connect(e);
        return 0;
    });
    sys::socket_address client_end;
    sys::socket client;
    sock.accept(client, client_end);
    client.unsetf(sys::open_flag::non_blocking);
    expect(no_throw(call([&] () { client.peer_name(); })));
    sys::user_credentials creds = client.credentials();
    expect(value(child.id()) == value(creds.pid));
    expect(value(sys::this_process::user()) == value(creds.uid));
    expect(value(sys::this_process::group()) == value(creds.gid));
    sys::process_status status = child.wait();
    if (!expect(value(0) == value(status.exit_code()))) {
        std::clog << "status=" << status << std::endl;
    }
}

void test_socket_send_file_descriptors() {
    const char* path = "\0testsendfds";
    sys::unix_socket_address e(path);
    sys::socket sock(sys::socket_address_family::unix);
    sock.bind(e);
    sock.unsetf(sys::open_flag::non_blocking);
    sock.listen();
    expect(value("") != value(test::stream_insert(sock)));
    sys::process child([&] () {
        sys::socket s(sys::socket_address_family::unix);
        s.unsetf(sys::open_flag::non_blocking);
        s.connect(e);
        std::clog << "sending fds: 0 1 2" << std::endl;
        sys::fd_type fds[3] = {0, 1, 2};
        int ret = 0;
        try {
            s.send_fds(fds, 3);
        } catch (const std::exception& err) {
            ++ret;
            std::cerr << "Exception: " << err.what() << std::endl;
        }
        return ret;
    });
    sys::socket_address client_end;
    sys::socket client;
    sock.accept(client, client_end);
    client.unsetf(sys::open_flag::non_blocking);
    sys::fd_type fds[3] = {0, 0, 0};
    expect(throws(call([&] () { client.receive_fds(fds, 10000); })));
    expect(throws(call([&] () { client.send_fds(fds, 10000); })));
    client.receive_fds(fds, 3);
    expect(value(fds[0]) > value(2));
    expect(value(fds[1]) > value(2));
    expect(value(fds[2]) > value(2));
    sys::process_status status = child.wait();
    expect(value(0) == value(status.exit_code()));
}

#if defined(UNISTDX_HAVE_TCP_USER_TIMEOUT)
void test_socket_user_timeout() {
    sys::socket sock(sys::socket_address_family::ipv4);
    sock.set(sys::socket::options::reuse_address);
    sock.bind(sys::ipv4_socket_address{{127,0,0,1}, 0});
    expect(no_throw(call([&] () { sock.set_user_timeout(std::chrono::seconds(7)); })));
}
#endif

void test_socket_bind_connect() {
    try {
        sys::socket sock(sys::socket_address_family::ipv4);
        sock.bind(sys::ipv4_socket_address{{127,0,0,1},0});
        sock.connect(sys::ipv4_socket_address{{127,0,0,1},0});
    } catch (const sys::bad_call& err) {
        if (!expect(value(std::errc::operation_in_progress) == value(err.errc()))) {
            std::clog << "err.what: " << err.what() << std::endl;
        }
    }
}
