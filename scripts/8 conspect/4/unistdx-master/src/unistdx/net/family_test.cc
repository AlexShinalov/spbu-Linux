/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2020 Ivan Gankevich

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

#include <unistdx/net/family>
#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/operator>

void test_family_insert_extract() {
    for (auto family : {sys::socket_address_family::unspecified,
        sys::socket_address_family::ipv4,
        sys::socket_address_family::ipv6,
        sys::socket_address_family::unix,
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        sys::socket_address_family::netlink,
        #endif
    }) {
        test::bstream_insert_extract(family);
    }
}

void test_family_print() {
    test::stream_insert_equals("inet", sys::socket_address_family::inet);
    test::stream_insert_equals("unknown", sys::socket_address_family(1000));
}

void test_family_bstream_insert_extract_fails() {
    test::bstream_insert_extract_fails(sys::socket_address_family(1000));
}
