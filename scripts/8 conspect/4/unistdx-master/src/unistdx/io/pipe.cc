/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2019, 2020 Ivan Gankevich

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

#include <unistdx/io/pipe>

#include <unistdx/base/check>
#include <unistdx/base/make_object>
#include <unistdx/bits/safe_calls>
#include <unistdx/config>

namespace {

    inline void
    safe_pipe(sys::fd_type fds[2]) {
        using namespace sys;
        #if defined(UNISTDX_HAVE_F_SETNOSIGPIPE) || \
            !defined(UNISTDX_HAVE_PIPE2)
        bits::global_lock_type lock(bits::fork_mutex);
        #endif
        #if defined(UNISTDX_HAVE_PIPE2)
        UNISTDX_CHECK(::pipe2(reinterpret_cast<int*>(fds), O_CLOEXEC | O_NONBLOCK));
        #else
        UNISTDX_CHECK(::pipe(fds));
        bits::set_mandatory_flags(fds[0]);
        bits::set_mandatory_flags(fds[1]);
        #endif
        #if defined(UNISTDX_HAVE_F_SETNOSIGPIPE)
        UNISTDX_CHECK(::fcntl(fds[1], F_SETNOSIGPIPE, 1));
        #endif
    }

}

void
sys::pipe::open() {
    this->close();
    static_assert(sizeof(_fds) == 2*sizeof(fd_type), "bad sys::fildes size");
    safe_pipe(reinterpret_cast<fd_type*>(this->_fds));
}

void
sys::pipe::close() {
    this->in().close();
    this->out().close();
}

std::ostream&
sys::operator<<(std::ostream& out, const pipe& rhs) {
    return out << make_object("in", rhs.out(), "out", rhs.in());
}

ssize_t
sys::splice::operator()(fildes& in, fildes& out, size_t n) {
    ssize_t ret;
    #if defined(UNISTDX_HAVE_SPLICE)
    ret = ::splice(in.fd(), nullptr, out.fd(), nullptr, n, this->_flags);
    #endif
    UNISTDX_CHECK_IO(ret);
    return ret;
}
