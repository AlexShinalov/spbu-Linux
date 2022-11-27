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
#include <cstring>
#include <stdexcept>

#include <unistdx/base/contracts>
#include <unistdx/base/sha2>
#include <unistdx/bits/macros>
#include <unistdx/config>
#include <unistdx/net/byte_order>

namespace {

constexpr const sys::u32 k_256[64] = {
0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

constexpr const sys::u64 k_512[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

    // circular right shift
    template <class T> UNISTDX_CONST T
    rotr(T x, int n) noexcept {
        return (x >> n) | (x << (sizeof(T)*8-n));
    }

    // right shift
    template <class T> UNISTDX_CONST T
    shr(T x, int n) noexcept {
        return x >> n;
    }

    template <class T> UNISTDX_CONST T
    ch(T x, T y, T z) noexcept {
        return (x & y) ^ (~x & z);
    }

    template <class T> UNISTDX_CONST T
    maj(T x, T y, T z) noexcept {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    UNISTDX_CONST sys::u32
    sigma_0_256(sys::u32 x) noexcept {
        return rotr(x,7) ^ rotr(x,18) ^ shr(x,3);
    }

    UNISTDX_CONST sys::u32
    sigma_1_256(sys::u32 x) noexcept {
        return rotr(x,17) ^ rotr(x,19) ^ shr(x,10);
    }

    UNISTDX_CONST sys::u32
    sum_0_256(sys::u32 x) noexcept {
        return rotr(x,2) ^ rotr(x,13) ^ rotr(x,22);
    }

    UNISTDX_CONST sys::u32
    sum_1_256(sys::u32 x) noexcept {
        return rotr(x,6) ^ rotr(x,11) ^ rotr(x,25);
    }

    UNISTDX_CONST sys::u64
    sigma_0_512(sys::u64 x) noexcept {
        return rotr(x,1) ^ rotr(x,8) ^ shr(x,7);
    }

    UNISTDX_CONST sys::u64
    sigma_1_512(sys::u64 x) noexcept {
        return rotr(x,19) ^ rotr(x,61) ^ shr(x,6);
    }

    UNISTDX_CONST sys::u64
    sum_0_512(sys::u64 x) noexcept {
        return rotr(x,28) ^ rotr(x,34) ^ rotr(x,39);
    }

    UNISTDX_CONST sys::u64
    sum_1_512(sys::u64 x) noexcept {
        return rotr(x,14) ^ rotr(x,18) ^ rotr(x,41);
    }

}

void sys::sha2_base::insert(const char* data, std::size_t n) {
    if (n >= std::numeric_limits<size_t>::max()/8 ||
        n >= std::numeric_limits<size_t>::max()/8 - this->_length/8) {
        throw std::length_error("sha2_base input is too large");
    }
    auto first = this->_blockptr, last = this->_block + 64;
    auto data_end = data + n;
    while (data != data_end) {
        const auto m = std::min(last-first, data_end-data);
        first = std::copy_n(data, m, first);
        data += m;
        if (first == last) {
            process_block();
            first = this->_block;
        }
    }
    this->_blockptr = first;
    this->_length += n*8;
}

void sys::sha2_base::finish() noexcept {
    pad_message();
}

void sys::sha2_base::process_block() noexcept {
    auto* w = this->_words;
    #if !defined(UNISTDX_BIG_ENDIAN)
    for (int i=0; i<16; ++i) {
        w[i] = byte_swap(w[i]);
    }
    #endif
    for (int i=16; i<64; ++i) {
        w[i] = sigma_1_256(w[i-2]) + w[i-7] + sigma_0_256(w[i-15]) + w[i-16];
    }
    u32 a = this->_digest[0];
    u32 b = this->_digest[1];
    u32 c = this->_digest[2];
    u32 d = this->_digest[3];
    u32 e = this->_digest[4];
    u32 f = this->_digest[5];
    u32 g = this->_digest[6];
    u32 h = this->_digest[7];
    for (int i=0; i<64; ++i) {
        u32 t1 = h + sum_1_256(e) + ch(e,f,g) + k_256[i] + w[i];
        u32 t2 = sum_0_256(a) + maj(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    this->_digest[0] += a,
    this->_digest[1] += b,
    this->_digest[2] += c,
    this->_digest[3] += d,
    this->_digest[4] += e,
    this->_digest[5] += f,
    this->_digest[6] += g,
    this->_digest[7] += h;
}

void sys::sha2_base::pad_message() noexcept {
    const u64 orig_length = this->_length;
    const int bytes_needed = sizeof(unsigned char) + sizeof(u64);
    const int bytes_avail = this->_block + 64 - this->_blockptr;
    *this->_blockptr++ = 0x80;
    if (bytes_avail < bytes_needed) {
        // there is not enough space for a 64-bit message size
        std::fill(this->_blockptr, this->_block + 64, '\0');
        this->process_block();
        this->_blockptr = this->_block;
    }
    // pad the block
    std::fill(this->_blockptr, this->_block + 64 - sizeof(u64), '\0');
    // store the size of the original message
    // in the last double word
    this->_dwords[7] = to_network_format(orig_length);
    this->process_block();
}

std::string sys::sha2_base::to_string(int n) const {
    std::string s;
    s.reserve(64);
    for (int i=0; i<n; ++i) {
        for (int j=1; j<=8; ++j) {
            s += "0123456789abcdef"[(this->_digest[i] >> (sizeof(u32)*8 - 4*j)) & 0xf];
        }
    }
    return s;
}

void sys::sha2_512_base::insert(const char* data, std::size_t n) {
    if (n >= std::numeric_limits<size_t>::max()/8 ||
        n >= std::numeric_limits<size_t>::max()/8 - this->_length.b/8) {
        throw std::length_error("sha2_512_base input is too large");
    }
    auto first = this->_blockptr, last = this->_block + sizeof(this->_block);
    auto data_end = data + n;
    while (data != data_end) {
        const auto m = std::min(last-first, data_end-data);
        first = std::copy_n(data, m, first);
        data += m;
        if (first == last) {
            process_block();
            first = this->_block;
        }
    }
    this->_blockptr = first;
    this->_length.b += n*8;
}

void sys::sha2_512_base::finish() noexcept {
    pad_message();
}

void sys::sha2_512_base::process_block() noexcept {
    auto* w = this->_words;
    #if !defined(UNISTDX_BIG_ENDIAN)
    for (int i=0; i<16; ++i) {
        w[i] = byte_swap(w[i]);
    }
    #endif
    for (int i=16; i<80; ++i) {
        w[i] = sigma_1_512(w[i-2]) + w[i-7] + sigma_0_512(w[i-15]) + w[i-16];
    }
    u64 a = this->_digest[0];
    u64 b = this->_digest[1];
    u64 c = this->_digest[2];
    u64 d = this->_digest[3];
    u64 e = this->_digest[4];
    u64 f = this->_digest[5];
    u64 g = this->_digest[6];
    u64 h = this->_digest[7];
    for (int i=0; i<80; ++i) {
        u64 t1 = h + sum_1_512(e) + ch(e,f,g) + k_512[i] + w[i];
        u64 t2 = sum_0_512(a) + maj(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    this->_digest[0] += a,
    this->_digest[1] += b,
    this->_digest[2] += c,
    this->_digest[3] += d,
    this->_digest[4] += e,
    this->_digest[5] += f,
    this->_digest[6] += g,
    this->_digest[7] += h;
}

void sys::sha2_512_base::pad_message() noexcept {
    const auto orig_length = this->_length;
    const int bytes_needed = sizeof(unsigned char) + sizeof(u64)*2;
    const int bytes_avail = this->_block + sizeof(this->_block) - this->_blockptr;
    *this->_blockptr++ = 0x80;
    if (bytes_avail < bytes_needed) {
        // there is not enough space for a 64-bit message size
        std::fill(this->_blockptr, this->_block + sizeof(this->_block), '\0');
        this->process_block();
        this->_blockptr = this->_block;
    }
    // pad the block
    std::fill(this->_blockptr, this->_block + sizeof(this->_block) - sizeof(u64)*2, '\0');
    // store the size of the original message
    // in the last double word
    #if defined(UNISTDX_BIG_ENDIAN)
    this->_dwords[7].a = orig_length.a;
    this->_dwords[7].b = orig_length.b;
    #else
    this->_dwords[7].a = to_network_format(orig_length.a);
    this->_dwords[7].b = to_network_format(orig_length.b);
    #endif
    this->process_block();
}

std::string sys::sha2_512_base::to_string(int n) const {
    std::string s;
    s.reserve(128);
    for (int i=0; i<n; ++i) {
        for (int j=1; j<=16; ++j) {
            s += "0123456789abcdef"[(this->_digest[i] >> (sizeof(u64)*8 - 4*j)) & 0xf];
        }
    }
    return s;
}

std::string sys::sha2_512_224::to_string() const {
    const auto* d = digest();
    std::string s;
    s.reserve(sizeof(u32)*7*2);
    for (int i=0; i<7; ++i) {
        for (int j=1; j<=8; ++j) {
            u32 a = d[i>>1] & UINT64_C(0xffffffff);
            u32 b = (d[i>>1] & UINT64_C(0xffffffff00000000)) >> 32;
            s += "0123456789abcdef"[(((i&1)?a:b) >> (sizeof(u32)*8 - 4*j)) & 0xf];
        }
    }
    return s;
}
