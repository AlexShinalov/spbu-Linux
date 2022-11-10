/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

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

#include <algorithm>
#include <limits>
#include <stdexcept>

#include "byte_order.hh"

namespace {

    using sys::u32;

    const u32 k_0_19 = 0x5a827999u;
    const u32 k_20_39 = 0x6ed9eba1u;
    const u32 k_40_59 = 0x8f1bbcdcu;
    const u32 k_60_79 = 0xca62c1d6u;

    // circular left shift
    u32
    cls(int n, u32 x) noexcept {
        return (x << n) | (x >> (32-n));
    }

    // logical functions {{{
    u32
    f_0_19(u32 b, u32 c, u32 d) noexcept {
        return (b & c) | ((~b) & d);
    }

    u32
    f_20_39(u32 b, u32 c, u32 d) noexcept {
        return b ^ c ^ d;
    }

    u32
    f_40_59(u32 b, u32 c, u32 d) noexcept {
        return (b & c) | (b & d) | (c & d);
    }

    u32
    f_60_79(u32 b, u32 c, u32 d) noexcept {
        return f_20_39(b, c, d);
    }

    // }}}

}

#define MAKE_LOOP(from, to) \
    for (int i=from; i<=(to); ++i) { \
        temp = cls(5, a) + f_ ## from ## _ ## to(b, c, d) + e + w[i] \
               + k_ ## from ## _ ## to; \
        e = d; \
        d = c; \
        c = cls(30, b); \
        b = a; \
        a = temp; \
    }

void
sys::sha1::process_block() noexcept {
    // init words
    u32* w = this->_words;
    #if !defined(UNISTDX_BIG_ENDIAN)
    for (int i=0; i<16; ++i) {
        w[i] = to_host_format(w[i]);
    }
    #endif
    for (int i=16; i<=79; ++i) {
        w[i] = cls(1, w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]);
    }
    // process the block
    u32 a=this->_digest[0];
    u32 b=this->_digest[1];
    u32 c=this->_digest[2];
    u32 d=this->_digest[3];
    u32 e=this->_digest[4];
    u32 temp;
    MAKE_LOOP(0, 19);
    MAKE_LOOP(20, 39);
    MAKE_LOOP(40, 59);
    MAKE_LOOP(60, 79);
    // update the digest
    this->_digest[0] += a;
    this->_digest[1] += b;
    this->_digest[2] += c;
    this->_digest[3] += d;
    this->_digest[4] += e;
}

#undef MAKE_LOOP

void
sys::sha1::xput(const char* s, const char* sn, std::size_t n) {
    if (n >= std::numeric_limits<size_t>::max()/8 ||
        n >= std::numeric_limits<size_t>::max()/8 - this->_length/8) {
        throw std::length_error("sha1 input is too large"); // LCOV_EXCL_LINE
    }
    unsigned char* first = this->_blockptr;
    unsigned char* last = this->block_end();
    while (s != sn) {
        const size_t m = std::min(last - first, sn - s);
        first = std::copy_n(s, m, first);
        s += m;
        if (first == last) {
            this->process_block();
            first = this->_block;
        }
    }
    this->_blockptr = first;
    this->_length += n*8;
}

void
sys::sha1::pad_block() noexcept {
    const u64 orig_length = this->_length;
    const int bytes_needed = sizeof(unsigned char) + sizeof(u64);
    const int bytes_avail = this->block_end() - this->_blockptr;
    *this->_blockptr++ = 0x80;
    if (bytes_avail < bytes_needed) {
        // there is not enough space for a 64-bit message size
        std::fill(this->_blockptr, this->block_end(), '\0');
        this->process_block();
        this->_blockptr = this->_block;
    }
    // pad the block
    std::fill(this->_blockptr, this->block_end() - sizeof(u64), '\0');
    // store the size of the original message
    // in the last double word
    this->_dwords[7] = to_network_format(orig_length);
    this->process_block();
}
