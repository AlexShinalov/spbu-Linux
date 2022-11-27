/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <unistdx/base/types>
#include <unistdx/net/bit_count>
#include <unistdx/test/language>

using namespace sys::test::lang;

void test_bit_count_unsigned_long() {
    std::bitset<8*sizeof(unsigned long)> bits;
    bits.set(1);
    bits.set(2);
    bits.set(3);
    bits.set(bits.size()-1);
    expect(value(bits.count()) == value(sys::bit_count<unsigned long>(bits.to_ulong())));
}

#if defined(UNISTDX_HAVE_LONG_LONG)
void test_bit_count_unsigned_long_long() {
    std::bitset<8*sizeof(unsigned long long)> bits;
    bits.set(1);
    bits.set(2);
    bits.set(3);
    bits.set(bits.size()-1);
    expect(value(bits.count()) == value(sys::bit_count<unsigned long long>(bits.to_ullong())));
}
#endif

void test_bit_count_unsigned_int() {
    expect(value(1u) == value(sys::bit_count<unsigned int>(1)));
    expect(value(1u) == value(sys::bit_count<unsigned int>(2)));
    expect(value(2u) == value(sys::bit_count<unsigned int>(3)));
}
