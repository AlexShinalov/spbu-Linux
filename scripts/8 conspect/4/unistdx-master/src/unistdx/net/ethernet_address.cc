/*
UNISTDX — C++ library for Linux system calls.
© 2019, 2020 Ivan Gankevich

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

#include <iomanip>
#include <istream>
#include <ostream>

#include <unistdx/base/ios_guard>
#include <unistdx/bits/addr_parse>
#include <unistdx/net/ethernet_address>

namespace {

    inline void
    pad_stream(std::ostream& out, const std::streamsize padding) {
        const char ch = out.fill();
        for (std::streamsize i=0; i<padding; ++i) {
            out.put(ch);
        }
    }

}

std::ostream&
sys::operator<<(std::ostream& out, const ethernet_address& rhs) {
    typedef ethernet_address::size_type size_type;
    ios_guard g(out);
    std::streamsize padding = out.width(0) - 17;
    bool pad_left = (out.flags() & std::ios::adjustfield) != std::ios::left;
    if (padding > 0 && pad_left) {
        out.fill(' ');
        pad_stream(out, padding);
    }
    out.fill('0');
    out.setf(std::ios::hex, std::ios::basefield);
    out << std::setw(2) << static_cast<u32>(rhs[0]);
    for (size_type i=1; i<ethernet_address::size(); ++i) {
        out << ':' << std::setw(2) << static_cast<u32>(rhs[i]);
    }
    if (padding > 0 && !pad_left) {
        out.fill(' ');
        pad_stream(out, padding);
    }
    return out;
}

std::istream&
sys::operator>>(std::istream& in, ethernet_address& rhs) {
    typedef bits::Num<ethernet_address::value_type, u32> Octet;
    typedef ethernet_address::size_type size_type;
    using bits::Colon;
    ios_guard g(in);
    in.setf(std::ios::hex, std::ios::basefield);
    Octet octets[ethernet_address::size()];
    in >> octets[0];
    for (size_type i=1; i<ethernet_address::size(); ++i) {
        in >> Colon() >> std::hex >> octets[i];
    }
    if (!in.fail()) {
        for (size_type i=0; i<ethernet_address::size(); ++i) {
            rhs[i] = octets[i];
        }
    }
    return in;
}
