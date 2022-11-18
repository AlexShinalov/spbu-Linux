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

#include "sha1.hh"

#include <chrono>
#include <iostream>

std::string a = "aaaaaaaaaa";
sys::sha1 sha;

void
compute_sha1() {
    for (int i=0; i<100000; i++) {
        sha.put(a.data(), a.size());
    }
    sha.compute();
    sha.reset();
    //sys::log_message("bnch", "sha1=_", result);
}

int main() {
    using namespace std::chrono;
    typedef high_resolution_clock clock_type;
    for (int i=0; i<1000; ++i) {
        auto t0 = clock_type::now();
        compute_sha1();
        auto t1 = clock_type::now();
//		sys::log_message("bnch", "t=_", (t1-t0).count());
        std::clog << duration_cast<microseconds>(t1-t0).count() << '\n';
    }
    return 0;
}
