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

#include <cstdlib>
#include <thread>
#include <unistdx/system/resource>

namespace {

    inline sys::size_type
    get_size(int name) {
        long result = ::sysconf(name);
        return result > 0 ? result : 0;
    }

}

unsigned
sys::thread_concurrency() noexcept {
    #if defined(UNISTDX_SINGLE_THREAD)
    return 1u;
    #else
    int concurrency = 0;
    const char* cc = std::getenv("UNISTDX_CONCURRENCY");
    if (cc) {
        concurrency = std::atoi(cc);
    }
    if (concurrency < 1) {
        concurrency = std::thread::hardware_concurrency();
    }
    if (concurrency < 1) {
        concurrency = 1; // LCOV_EXCL_LINE
    }
    return static_cast<unsigned>(concurrency);
    #endif
}

sys::cache::cache() {
    const int names[4][3] = {
        {_SC_LEVEL1_DCACHE_SIZE, _SC_LEVEL1_DCACHE_LINESIZE, _SC_LEVEL1_DCACHE_ASSOC},
        {_SC_LEVEL2_CACHE_SIZE, _SC_LEVEL2_CACHE_LINESIZE, _SC_LEVEL2_CACHE_ASSOC},
        {_SC_LEVEL3_CACHE_SIZE, _SC_LEVEL3_CACHE_LINESIZE, _SC_LEVEL3_CACHE_ASSOC},
        {_SC_LEVEL4_CACHE_SIZE, _SC_LEVEL4_CACHE_LINESIZE, _SC_LEVEL4_CACHE_ASSOC},
    };
    int nlevels = 0;
    for (int i=0; i<4; ++i) {
        size_type s = get_size(names[i][0]);
        if (s == 0) {
            break;
        }
        this->_levels[i]._level = i + 1;
        this->_levels[i]._size = s;
        this->_levels[i]._linesize = get_size(names[i][1]);
        this->_levels[i]._assoc = get_size(names[i][2]);
        ++nlevels;
    }
    this->_nlevels = nlevels;
}
