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

#include <unistdx/fs/copy_file>

#include <unistdx/config>
#include <unistdx/fs/file_status>
#include <unistdx/io/fildes>

#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
#include <unistd.h>
#endif
#if defined(UNISTDX_HAVE_SENDFILE)
#include <sys/sendfile.h>
#endif

namespace {

    inline void
    do_simple_copy(
        sys::fildes& in,
        sys::fildes& out,
        sys::offset_type file_size
    ) {
        const size_t n = 4096;
        char buf[n];
        ssize_t nread = 0;
        do {
            ssize_t m = in.read(buf, n);
            nread += m;
            ssize_t nwritten = 0;
            do { // LCOV_EXCL_LINE
                nwritten += out.write(buf, m);
            } while (nwritten != m);
        } while (nread != file_size);
    }

    #if defined(UNISTDX_HAVE_SENDFILE)
    void
    do_sendfile(
        sys::fildes& in,
        sys::fildes& out,
        sys::offset_type size
    ) {
        sys::fd_type ifd = in.fd();
        sys::fd_type ofd = out.fd();
        sys::offset_type offset = 0;
        ssize_t n;
        while ((n = ::sendfile(ofd, ifd, &offset, size)) > 0) {
            size -= n;
        }
        if (size != 0) {
            if (errno != EINVAL && errno != ENOSYS) {
                throw sys::bad_call(); // LCOV_EXCL_LINE
            }
            do_simple_copy(in, out, size);
        }
    }
    #endif

    #if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
    inline void
    do_copy_file_range(
        sys::fildes& in,
        sys::fildes& out,
        sys::offset_type size
    ) {
        sys::fd_type ifd = in.fd();
        sys::fd_type ofd = out.fd();
        ssize_t n;
        while ((n = ::copy_file_range(ifd, nullptr, ofd, nullptr, size, 0)) > 0) {
            size -= n;
        }
        if (size != 0) {
            if (errno != EXDEV && errno != EBADF && errno != ENOSYS) {
                throw sys::bad_call(); // LCOV_EXCL_LINE
            }
            #if defined(UNISTDX_HAVE_SENDFILE)
            do_sendfile(in, out, size);
            #else
            do_simple_copy(in, out, size);
            #endif
        }
    }
    #endif

}

void
sys::copy_file(const path& src, const path& dest) {
    fildes in(
        src,
        open_flag::read_only |
        open_flag::close_on_exec,
        0644
    );
    fildes out(
        dest,
        open_flag::create |
        open_flag::truncate |
        open_flag::write_only |
        open_flag::close_on_exec,
        0644
    );
    file_status st(src);
    const offset_type size = st.size();
    #if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
    do_copy_file_range(in, out, size);
    #elif defined(UNISTDX_HAVE_SENDFILE)
    do_sendfile(in, out, size);
    #else
    do_simple_copy(in, out, size);
    #endif
}
