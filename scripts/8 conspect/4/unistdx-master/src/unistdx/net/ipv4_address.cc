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

#include <unistdx/net/ipv4_address>

#include <istream>
#include <ostream>
#include <sstream>

#include <unistdx/base/types>
#include <unistdx/bits/addr_parse>

namespace {

    inline int
    num_digits(sys::ipv4_address::value_type rhs) noexcept {
        return rhs >= 100 ? 3 :
               rhs >= 10  ? 2 :
               1;
    }

    inline std::streamsize
    width(sys::ipv4_address a) noexcept {
        return num_digits(a[0]) +
               num_digits(a[1]) +
               num_digits(a[2]) +
               num_digits(a[3]) +
               3;
    }

    inline void
    pad_stream(std::ostream& out, const std::streamsize padding) {
        const char ch = out.fill();
        for (std::streamsize i=0; i<padding; ++i) {
            out.put(ch);
        }
    }

    typedef sys::bits::Number<sys::u8, sys::u16> Octet;

}

std::ostream&
sys::operator<<(std::ostream& out, ipv4_address rhs) {
    using bits::Dot;
    const std::streamsize padding = out.width(0) - width(rhs);
    const bool pad_left = (out.flags() & std::ios_base::adjustfield) !=
                          std::ios_base::left;
    if (padding > 0 && pad_left) {
        pad_stream(out, padding);
    }
    out << Octet{rhs._octets[0]} << Dot{};
    out << Octet{rhs._octets[1]} << Dot{};
    out << Octet{rhs._octets[2]} << Dot{};
    out << Octet{rhs._octets[3]};
    if (padding > 0 && !pad_left) {
        pad_stream(out, padding);
    }
    return out;
}

std::istream&
sys::operator>>(std::istream& in, ipv4_address& rhs) {
    using bits::Dot;
    in >> Octet{rhs._octets[0]} >> Dot{};
    in >> Octet{rhs._octets[1]} >> Dot{};
    in >> Octet{rhs._octets[2]} >> Dot{};
    in >> Octet{rhs._octets[3]};
    if (in.fail()) {
        rhs.clear();
    }
    return in;
}

sys::ipv4_address::ipv4_address(const char* rhs) {
    std::stringstream tmp(rhs);
    tmp >> *this;
    if (tmp.fail()) {
        throw std::invalid_argument("bad ipv4_address");
    }
}
