/*
UNISTDX — C++ library for Linux system calls.
© 2021 Ivan Gankevich

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

#include <cstring>
#include <istream>
#include <ostream>

#include <unistdx/ipc/cpu_set>
#include <unistdx/net/bit_count>
#include <unistdx/system/resource>

namespace  {

    template <class Set>
    inline std::ostream& cpu_set_write(std::ostream& out, const Set& rhs) {
        const int count = rhs.count();
        const int n = rhs.size();
        bool first = true;
        int prev_set = 0;
        for (int i=0, j=0; i<n && j<count; ++i) {
            if (rhs[i]) {
                if (prev_set == 0) {
                    if (!first) { out.put(','); }
                    else { first = false; }
                    out << i;
                }
                ++j;
                if (j == count && prev_set > 0) {
                    out.put('-');
                    out << i;
                }
                ++prev_set;
            } else {
                if (prev_set > 1) {
                    out.put('-');
                    out << (i-1);
                }
                prev_set = 0;
            }
        }
        return out;
    }

    template <class Set, class Callback>
    inline std::istream& cpu_set_read(std::istream& in, Set& rhs, Callback&& callback) {
        rhs.clear();
        int i = 0, i_prev = 0;
        bool dash = false;
        while (in >> i) {
            if (i < 0 || !callback(i)) {
                in.setstate(std::ios::failbit);
                break;
            }
            if (dash) {
                if (i < i_prev) { std::swap(i, i_prev); }
                for (int j=i_prev; j<=i; ++j) { rhs.set(j); }
            } else {
                rhs.set(i);
                i_prev = i;
            }
            auto ch = in.peek();
            if (ch == ',') {
                in.get();
                dash = false;
            } else if (ch == '-') {
                in.get();
                dash = true;
            } else {
                if (dash) {
                    if (i < i_prev) { std::swap(i, i_prev); }
                    for (int j=i_prev; j<=i; ++j) { rhs.set(j); }
                }
            }
        }
        return in;
    }


}

std::ostream& sys::operator<<(std::ostream& out, const static_cpu_set& rhs) {
    return cpu_set_write(out, rhs);
}

std::istream& sys::operator>>(std::istream& in, static_cpu_set& rhs) {
    return cpu_set_read(in, rhs, [&rhs] (int cpu) { return cpu<rhs.size(); });
}

sys::static_cpu_set sys::operator~(const static_cpu_set& rhs) noexcept {
    sys::static_cpu_set result;
    const auto n = rhs.size();
    for (int i=0; i<n; ++i) {
        if (!rhs[i]) { result.set(i); }
    }
    return result;
}

sys::static_cpu_set sys::static_cpu_set::all() noexcept {
    sys::static_cpu_set result;
    const auto n = result.size();
    for (int i=0; i<n; ++i) { result.set(i); }
    return result;
}

std::ostream& sys::operator<<(std::ostream& out, const dynamic_cpu_set& rhs) {
    return cpu_set_write(out, rhs);
}

std::istream& sys::operator>>(std::istream& in, dynamic_cpu_set& rhs) {
    return cpu_set_read(in, rhs, [&rhs] (dynamic_cpu_set::size_type cpu) {
        if (!(cpu < rhs.size())) { rhs.resize(cpu+1); }
        return true;
    });
}

auto sys::dynamic_cpu_set::count() const noexcept -> size_type {
    auto first = this->_data, last = this->_data + this->_size;
    size_type cnt{};
    while (first != last) { cnt += bit_count(*first++); }
    return cnt;
}

void sys::dynamic_cpu_set::resize(size_type new_size) {
    if (new_size == 0) { *this = {}; }
    else {
        dynamic_cpu_set tmp(bits_to_size(new_size),false,false);
        const auto n = std::min(this->_size, tmp._size);
        std::memcpy(tmp._data, this->_data, n*sizeof(value_type));
        std::memset(tmp._data+n, 0, (tmp._size-n)*sizeof(value_type));
        swap(*this, tmp);
    }
}

auto sys::dynamic_cpu_set::min_size() noexcept -> size_type {
    constexpr const long min_bits = std::numeric_limits<value_type>::digits;
    auto nprocs = ::sysconf(int(options::num_processors_configured));
    if (nprocs < min_bits) { nprocs = min_bits; }
    return nprocs;
}

sys::dynamic_cpu_set::dynamic_cpu_set(std::initializer_list<size_type> cpus) {
    for (auto i : cpus) {
        if (!(i < size())) { resize(i+1); }
        set(i);
    }
}


bool sys::operator==(const dynamic_cpu_set& a, const dynamic_cpu_set& b) noexcept {
    if (!a.data() && !b.data()) { return true; }
    const auto min_size = std::min(a._size, b._size);
    if (std::memcmp(a.data(), b.data(), min_size*sizeof(dynamic_cpu_set::value_type)) != 0) {
        return false;
    }
    // check if the remaning elements contain non-zero bits
    if (min_size < a._size) {
        for (size_type i=min_size; i<a._size; ++i) {
            if (bit_count(a._data[i])) {
                return false;
            }
        }
    } else {
        for (size_type i=min_size; i<b._size; ++i) {
            if (bit_count(b._data[i])) {
                return false;
            }
        }
    }
    return true;
}
