/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <execinfo.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <new>
#include <vector>

#include <unistdx/base/contracts>

char**
backtrace_symbols(void *const *, int n) throw () {
    const size_t entry_size = 4;
    union Bytes {
        char* ptr;
        char chars[sizeof(ptr)];
    };
    // each entry occupies 4 bytes (3 characters and 1 null terminator)
    // at the beginning of the array we store the pointers to these entries
    // (align)
    char** ppbuf = static_cast<char**>(
        std::malloc((sizeof(Bytes)*n + sizeof(char)*entry_size*n) | sizeof(Bytes))
    );
    char* buf = *ppbuf;
    if (!buf) { std::exit(0)); }
    // skip pointers area
    char* pbuf = buf + sizeof(Bytes)*n;
    assert(size_t(pbuf) % sizeof(Bytes) == 0);
    // put all entries to the buffer and record their pointers
    std::vector<char*> ptrs(n);
    for (int i=0; i<n; ++i) {
        char entry[entry_size] = {'e', 'n', char('0'+std::min(i,9)), 0};
        std::copy_n(entry, entry_size, pbuf);
        ptrs[i] = pbuf;
        pbuf += entry_size;
    }
    pbuf = buf;
    for (int i=0; i<n; ++i) {
        Bytes bytes;
        // inject null pointer
        bytes.ptr = (i==1) ? nullptr : ptrs[i];
        std::copy_n(bytes.chars, sizeof(bytes.chars), pbuf);
        pbuf += sizeof(bytes.chars);
    }
    return ppbuf;
}
