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

#include <algorithm>
#include <locale>
#include <ostream>

#include <unistdx/base/uint128>

namespace {

    constexpr static char alphabet[] = {
        '0','1','2','3','4','5','6','7','8','9',
        'a','b','c','d','e','f'};

    std::string
    u128_to_string(const sys::u128& rhs, int radix, size_t width, char fill, bool left,
                   bool plus) {
        std::string result;
        result.reserve(std::max(size_t(43),width));
        sys::u128 r(radix);
        sys::u128_div d{rhs,0};
        while (d.quot) {
            d = sys::div(d.quot, r);
            result += alphabet[d.rem.b()];
        }
        if (result.empty()) { result += '0'; }
        if (plus) { result += '+'; }
        std::reverse(result.begin(), result.end());
        auto s = result.size();
        if (s < width) { result.insert(left ? s : 0, width-s, fill); }
        return result;
    }

}

std::string sys::to_string(const u128& rhs) {
    return u128_to_string(rhs, 10, 0, ' ', false, false);
}

std::ostream& sys::operator<<(std::ostream& out, const u128& rhs) {
    int radix = 10;
    switch (out.flags() & std::ios::basefield) {
        case std::ios::oct: radix = 8; break;
        case std::ios::hex: radix = 16; break;
        default: break;
    }
    auto&& s = u128_to_string(rhs, radix, out.width(), out.fill(),
                              out.flags() & std::ios::left,
                              out.flags() & std::ios::showpos);
    if ((out.flags() & std::ios::uppercase) && (out.flags() & std::ios::hex)) {
        for (auto& ch : s) { ch = std::toupper(ch, out.getloc()); }
    }
    return out << s;
}

namespace std {
    constexpr int numeric_limits<sys::u128>::digits;
}
