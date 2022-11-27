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

#include <unistdx/net/ipv6_address>

#include <istream>
#include <ostream>
#include <sstream>

#include <unistdx/base/ios_guard>
#include <unistdx/base/types>
#include <unistdx/bits/addr_parse>

std::ostream&
sys::operator<<(std::ostream& out, const ipv6_address& rhs) {
    std::ostream::sentry s(out);
    if (s) {
        typedef ipv6_address::hex_type hex_type;
        typedef std::ostream::char_type char_type;
        ios_guard g(out);
        out.setf(std::ios::hex, std::ios::basefield);
        std::copy_n(
            rhs._hextets,
            rhs.num_hextets(),
            intersperse_iterator<hex_type,char_type>(out, ':')
        );
    }
    return out;
}

std::istream&
sys::operator>>(std::istream& in, ipv6_address& rhs) {
    std::istream::sentry s(in);
    if (!s) {
        return in;
    }
    typedef ipv6_address::hex_type hex_type;
    typedef bits::Number<u16, u32> Hextet;
    ios_guard g(in);
    in.setf(std::ios::hex, std::ios::basefield);
    int field_no = 0;
    int zeros_field = -1;
    for (hex_type& field : rhs._hextets) {
        if (in.fail()) break;
        // compressed notation
        if (in.peek() == ':') {
            in.get();
            if (field_no == 0) {
                in >> bits::Colon();
            }
            if (zeros_field != -1) {
                in.setstate(std::ios::failbit);
            } else {
                zeros_field = field_no;
            }
        }
        if (in >> Hextet{field}) {
            char ch = in.peek();
            // if prefixed with ::ffff:
            if (field_no >= 1 && rhs._hextets[0] == 0xffff && zeros_field == 0) {
                in >> bits::Dot();
            } else {
                in >> bits::Colon();
            }
            // put back the first character after the address
            if (in.fail()) {
                in.clear();
                in.putback(ch);
            }
            ++field_no;
        }
    }
    // push fields after :: towards the end
    if (zeros_field != -1) {
        in.clear();
        auto zeros_start = rhs._hextets + zeros_field;
        auto zeros_end = rhs._hextets + rhs.num_hextets() - (field_no - zeros_field);
        std::copy(zeros_start, rhs._hextets + field_no, zeros_end);
        std::fill(zeros_start, zeros_end, 0);
    }
    if (in.fail()) {
        rhs.clear();
    } else {
        // we do not need to change byte order here
        // rhs._bytes.to_network_format();
    }
    return in;
}

void
sys::ipv6_address::clear() {
    std::fill_n(this->begin(), this->size(), value_type{0});
}

sys::ipv6_address::ipv6_address(const char* rhs) {
    std::stringstream tmp(rhs);
    tmp >> *this;
    if (tmp.fail()) {
        throw std::invalid_argument("bad ipv6_address");
    }
}
