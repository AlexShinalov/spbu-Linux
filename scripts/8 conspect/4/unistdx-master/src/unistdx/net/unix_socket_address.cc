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

#include <unistdx/bits/addr_parse>
#include <unistdx/net/unix_socket_address>

namespace {

    sys::socket_length_type path_length(const char* p) noexcept {
        using traits_type = std::char_traits<char>;
        sys::socket_length_type n = 0;
        if (!*p) { ++p; ++n; }
        n += traits_type::length(p);
        return n;
    }

}

sys::socket_length_type sys::unix_socket_address::path_length() const noexcept {
    return ::path_length(path());
}

void sys::unix_socket_address::path(const char* rhs) noexcept {
    using traits_type = std::char_traits<char>;
    this->_address.sin6_family = sa_family_type(socket_address_family::unix);
    sys::socket_length_type n = ::path_length(rhs);
    n = std::min(max_length(), n);
    auto dst = path();
    traits_type::copy(dst, rhs, n);
    dst[n] = 0;
}

std::ostream&
sys::operator<<(std::ostream& out, const unix_socket_address& rhs) {
    const char* p = rhs.path();
    if (!*p) { out << '@'; ++p; }
    return out << p;
}

std::istream&
sys::operator>>(std::istream& in, unix_socket_address& rhs) {
    using bits::Colon;
    std::istream::sentry s(in);
    if (s) {
        std::string path;
        if (in >> path) {
            if (path.empty()) {
                in.setstate(std::ios::failbit);
            } else {
                if (path.front() == '@') { path[0] = 0; }
                rhs.path(path.data());
            }
        }
    }
    return in;
}

sys::bstream&
sys::operator<<(bstream& out, const unix_socket_address& rhs) {
    u32 n = rhs.path_length();
    out << n;
    out.write(rhs.path(), n);
    return out;
}

sys::bstream&
sys::operator>>(bstream& in, unix_socket_address& rhs) {
    rhs._address.sin6_family = sa_family_type(socket_address_family::unix);
    u32 n = 0;
    in >> n;
    n = std::min(unix_socket_address::max_length(), n);
    in.read(rhs.path(), n);
    return in;
}
