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

#include <unistdx/fs/mkdirs>

#include <sys/stat.h>
#include <sys/types.h>

#include <unistdx/base/check>
#include <unistdx/fs/file_status>

void
sys::mkdirs(sys::path dir, file_mode m, path::size_type offset) {
    const sys::path::size_type n = dir.size();
    if (n == 0) {
        return;
    }
    for (sys::path::size_type i=offset; i<n-1; ++i) {
        const char ch = dir[i];
        if (ch == '/') {
            dir[i] = '\0';
            int ret = ::mkdir(dir, m);
            dir[i] = ch;
            if (ret == -1 && errno != EEXIST) {
                throw bad_call();
            }
        }
    }
    int ret = ::mkdir(dir, m);
    if (ret == -1) {
        if (errno != EEXIST) {
            throw bad_call();
        }
        file_status st(dir, path_flag::no_follow);
        if (!st.is_directory()) {
            throw bad_call(std::errc::not_a_directory);
        }
    }
}
