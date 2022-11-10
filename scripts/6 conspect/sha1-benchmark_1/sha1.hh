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

#ifndef UNISTDX_BASE_SHA1
#define UNISTDX_BASE_SHA1

#include "types.hh"

namespace sys {

    /**
    \brief Computes bytes digest using SHA-1 algorithm.
    \date 2018-05-21
    \see \rfc{3174}
    \details Computes bytes digest using US Secure Hash Algorithm 1 (SHA-1).
    */
    class sha1 {

    private:
        static const u32 h0 = 0x67452301;
        static const u32 h1 = 0xefcdab89;
        static const u32 h2 = 0x98badcfe;
        static const u32 h3 = 0x10325476;
        static const u32 h4 = 0xc3d2e1f0;

    private:
        union {
            unsigned char _block[64];
            u32 _words[80];
            u64 _dwords[8];
        };
        union {
            u32 _digest[5];
            unsigned char _bytes[20];
            char _chars[20];
        };
        unsigned char* _blockptr;
        std::size_t _length = 0;
        bool _computed = false;

    public:

        /// Construct SHA-1 digest object.
        inline
        sha1() noexcept:
        _digest{h0, h1, h2, h3, h4},
        _blockptr{_block}
        {}

        /// Reset SHA-1 object to start computing new hash.
        inline void
        reset() {
            this->_digest[0] = h0;
            this->_digest[1] = h1;
            this->_digest[2] = h2;
            this->_digest[3] = h3;
            this->_digest[4] = h4;
            this->_blockptr = this->_block;
            this->_length = 0;
            this->_computed = false;
        }

        /**
        \brief Append next \p n bytes from array pointed by \p first
        and compute digest if possible.
        \throws std::length_error if input size is too large to compute SHA-1
        on this system
        */
        inline void
        put(const char* first, std::size_t n) {
            this->xput(first, first+n, n);
        }

        /**
        \brief Append next byte array between \p first and \p last
        and compute digest if possible.
        \throws std::length_error if input size is too large to compute SHA-1
        on this system
        */
        inline void
        put(const char* first, const char* last) {
            this->xput(first, last, last-first);
        }

        /**
        Compute the final digest by padding the last block
        if needed. If the block does not require padding this
        method does nothing.
        */
        inline void
        compute() noexcept {
            if (!this->_computed) {
                this->pad_block();
                this->_computed = true;
            }
        }

        /// Copy SHA-1 digest to byte array pointed by \p result.
        inline void
        digest(unsigned char* result) noexcept {
            for (int i=0; i<digest_bytes_length(); ++i) {
                result[i] = this->_bytes[i];
            }
        }

        /// Copy SHA-1 digest to character array pointed by \p result.
        inline void
        digest(char* result) noexcept {
            for (int i=0; i<digest_bytes_length(); ++i) {
                result[i] = this->_bytes[i];
            }
        }

        /// Copy SHA-1 digest to unsigned integer array pointed by \p result.
        inline void
        digest(u32* result) noexcept {
            for (int i=0; i<digest_length(); ++i) {
                result[i] = this->_digest[i];
            }
        }

        /// Get SHA-1 digest as a pointer to unsigned integer array.
        inline const u32*
        digest() const noexcept {
            return this->_digest;
        }

        /// Get SHA-1 digest as a pointer to unsigned byte array.
        inline const unsigned char*
        digest_bytes() const noexcept {
            return this->_bytes;
        }

        /// Get SHA-1 digest as a pointer to character array.
        inline const char*
        digest_chars() const noexcept {
            return this->_chars;
        }

        /// Get the length of SHA1 input in bits.
        inline std::size_t
        length() const noexcept {
            return this->_length;
        }

        /// Returns 5.
        static constexpr inline int
        digest_length() noexcept {
            return 5;
        }

        /// Returns 20.
        static constexpr inline int
        digest_bytes_length() noexcept {
            return 20;
        }

    private:

        void
        xput(const char* s, const char* sn, std::size_t n);

        void
        process_block() noexcept;

        void
        pad_block() noexcept;

        inline unsigned char*
        block_begin() noexcept {
            return this->_block;
        }

        inline unsigned char*
        block_end() noexcept {
            return this->_block + 64;
        }

    };

}

#endif // vim:filetype=cpp
