/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020, 2021 Ivan Gankevich

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

#include <unistdx/net/socket>

#include <unistdx/base/log_message>
#include <unistdx/base/make_object>
#include <unistdx/bits/safe_calls>
#include <unistdx/config>

namespace {

    constexpr const int default_flags =
        UNISTDX_SOCK_NONBLOCK | UNISTDX_SOCK_CLOEXEC;

    inline int
    safe_socket(int domain, int type, int protocol) {
        using namespace sys::bits;
        #if !defined(UNISTDX_HAVE_SOCK_NONBLOCK) || \
        !defined(UNISTDX_HAVE_SOCK_CLOEXEC)
        global_lock_type lock(fork_mutex);
        #endif
        int sock;
        UNISTDX_CHECK(sock = ::socket(domain, type, protocol));
        #if !defined(UNISTDX_HAVE_SOCK_NONBLOCK) || \
        !defined(UNISTDX_HAVE_SOCK_CLOEXEC)
        set_mandatory_flags(sock);
        #endif
        return sock;
    }

    inline int
    safe_accept(int server_fd, sys::socket_address& client_address) {
        using namespace sys::bits;
        sys::socket_length_type len = sizeof(sys::socket_address);
        int fd = -1;
        #if defined(UNISTDX_HAVE_ACCEPT4) && \
            defined(UNISTDX_HAVE_SOCK_NONBLOCK) && \
            defined(UNISTDX_HAVE_SOCK_CLOEXEC)
        fd = ::accept4(server_fd, client_address.get(), &len,
                       UNISTDX_SOCK_NONBLOCK | UNISTDX_SOCK_CLOEXEC);
        #else
        global_lock_type lock(fork_mutex);
        fd = ::accept(server_fd, client_address.get(), &len);
        set_mandatory_flags(fd);
        #endif
        return fd;
    }

}

sys::socket::socket(socket_address_family family, socket_type type, protocol_type proto):
sys::fildes(safe_socket(int(family), int(type)|default_flags, proto))
{}

bool
sys::socket::accept(socket& sock, socket_address& addr) {
    auto client_fd = safe_accept(this->_fd, addr);
    if (client_fd == -1) {
        #if EAGAIN == EWOULDBLOCK
        if (errno == EAGAIN) { return false; }
        #else
        if (errno == EAGAIN || errno == EWOULDBLOCK) { return false; }
        #endif
        throw bad_call();
    }
    sock.close();
    sock._fd = client_fd;
    return true;
}

std::ostream&
sys::operator<<(std::ostream& out, const socket& rhs) {
    auto error = rhs.get<int>(socket::options::error);
    return out << make_object("fd", rhs._fd,
                              "status", std::make_error_code(std::errc(error)).message());
}

#if defined(UNISTDX_HAVE_SCM_RIGHTS)
namespace {
    union fds_message {
        sys::cmessage_header h;
        char bytes[CMSG_SPACE(64*sizeof(sys::fd_type))];
    };
}

void
sys::socket::send_fds(const sys::fd_type* data, size_t n) {
    if (n > 64) {
        throw std::invalid_argument("too many fds to send");
    }
    char dummy[1] = {0};
    const size_t size = n*sizeof(sys::fd_type);
    fds_message m{};
    m.h.cmsg_len = CMSG_LEN(size);
    m.h.cmsg_level = SOL_SOCKET;
    m.h.cmsg_type = SCM_RIGHTS;
    message_header h{};
    h.msg_control = m.bytes;
    h.msg_controllen = CMSG_SPACE(size);
    io_vector v{dummy, 1};
    h.msg_iov = &v;
    h.msg_iovlen = 1;
    h.msg_name = nullptr;
    h.msg_namelen = 0;
    std::memcpy(CMSG_DATA(&m.h), data, n*sizeof(fd_type));
    this->send(h);
}

void
sys::socket::receive_fds(sys::fd_type* data, size_t n) {
    if (n > 64) {
        throw std::invalid_argument("too many fds to receive");
    }
    const size_t size = n*sizeof(sys::fd_type);
    char dummy[1] = {0};
    io_vector v {dummy, 1};
    fds_message m;
    m.h.cmsg_len = CMSG_LEN(size);
    m.h.cmsg_level = 0;
    m.h.cmsg_type = 0;
    message_header h;
    h.msg_control = m.bytes;
    h.msg_controllen = CMSG_SPACE(size);
    h.msg_iov = &v;
    h.msg_iovlen = 1;
    h.msg_name = nullptr;
    h.msg_namelen = 0;
    this->receive(h);
    if (m.h.cmsg_level == SOL_SOCKET && m.h.cmsg_type == SCM_RIGHTS) {
        std::memcpy(data, CMSG_DATA(&m.h), n*sizeof(fd_type));
    }
}
#endif
