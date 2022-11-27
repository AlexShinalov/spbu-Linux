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

#include <iostream>
#include <sstream>
#include <string>

#include <unistdx/base/uint128>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_uint128_limits() {
    using namespace sys::literals;
    using sys::u128;
    expect(value(340282366920938463463374607431768211455_u128) ==
           value(std::numeric_limits<u128>::max()));
    expect(value(0_u128) == value(std::numeric_limits<u128>::min()));
    expect(value(128) == value(std::numeric_limits<u128>::digits));
}

using sys::u128;
using sys::u64;

void test_uint128_comparisons() {
    // one component
    expect(value(u128()) == value(u128(0)));
    expect(value(u128(0)) < value(u128(1)));
    expect(value(u128(0)) <= value(u128(1)));
    expect(value(u128(0)) <= value(u128(0)));
    expect(value(u128(1)) > value(u128(0)));
    expect(value(u128(1)) >= value(u128(0)));
    expect(value(u128(0)) >= value(u128(0)));
    expect(value(u128(0)) != value(u128(1)));
    // two components
    expect(value(u128()) == value(u128(0,0)));
    expect(value(u128(0,1)) < value(u128(1,0)));
    expect(value(u128(0,1)) <= value(u128(1,0)));
    expect(value(u128(1,0)) <= value(u128(1,0)));
    expect(value(u128(1,0)) > value(u128(0,1)));
    expect(value(u128(1,0)) >= value(u128(0,1)));
    expect(value(u128(1,0)) >= value(u128(1,0)));
    expect(value(u128(1,0)) != value(u128(0,1)));
}

void test_uint128_operator_bool() {
    expect(bool(u128(0,1)));
    expect(bool(u128(1,0)));
    expect(!bool(u128(0,0)));
    expect(!value(!u128(0,1)));
    expect(!value(!u128(1,0)));
    expect(!u128(0,0));
}

void test_uint128_operator_tilde() {
    expect(value(u128(0)) == value(~std::numeric_limits<u128>::max()));
    expect(value(~u128(0)) == value(std::numeric_limits<u128>::max()));
}

void test_uint128_operator_minus() {
    expect(value(-u64(0)) == value(u64(0)));
    expect(value(-u64(1)) == value(std::numeric_limits<u64>::max()));
    expect(value(-u64(2)) == value(std::numeric_limits<u64>::max()-1));
    expect(value(-u128(0)) == value(u128(0)));
    expect(value(-u128(1)) == value(std::numeric_limits<u128>::max()));
    expect(value(-u128(2)) == value(std::numeric_limits<u128>::max()-1));
}

void test_uint128_operator_bitwise_or() {
    expect(value(u128(0)) == value(u128(0) | u128(0)));
    expect(value(u128(1)) == value(u128(0) | u128(1)));
    expect(value(u128(1)) == value(u128(1) | u128(0)));
    expect(value(u128(1)) == value(u128(1) | u128(1)));
    u128 x;
    x = 0;
    expect(value(u128(0)) == value(x |= 0));
    x = 0;
    expect(value(u128(1)) == value(x |= 1));
    x = 1;
    expect(value(u128(1)) == value(x |= 0));
    x = 1;
    expect(value(u128(1)) == value(x |= 1));
}

void test_uint128_operator_bitwise_and() {
    expect(value(u128(0)) == value(u128(0) & u128(0)));
    expect(value(u128(0)) == value(u128(0) & u128(1)));
    expect(value(u128(0)) == value(u128(1) & u128(0)));
    expect(value(u128(1)) == value(u128(1) & u128(1)));
    u128 x;
    x = 0;
    expect(value(u128(0)) == value(x &= 0));
    x = 0;
    expect(value(u128(0)) == value(x &= 1));
    x = 1;
    expect(value(u128(0)) == value(x &= 0));
    x = 1;
    expect(value(u128(1)) == value(x &= 1));
}

void test_uint128_operator_bitwise_xor() {
    expect(value(u128(0)) == value(u128(0) ^ u128(0)));
    expect(value(u128(1)) == value(u128(0) ^ u128(1)));
    expect(value(u128(1)) == value(u128(1) ^ u128(0)));
    expect(value(u128(0)) == value(u128(1) ^ u128(1)));
    u128 x;
    x = 0;
    expect(value(u128(0)) == value(x ^= 0));
    x = 0;
    expect(value(u128(1)) == value(x ^= 1));
    x = 1;
    expect(value(u128(1)) == value(x ^= 0));
    x = 1;
    expect(value(u128(0)) == value(x ^= 1));
}

void test_uint128_operator_shift_left() {
    for (int i=0; i<64; ++i) {
        u64 y = u64(1)<<u64(i);
        if (!expect(value(u128(0,y)) == value(u128(1)<<u128(i)))) {
            std::clog << "i=" << i;
        }
        u128 x(1);
        x <<= u128(i);
        if (!expect(value(u128(0,y)) == value(x))) {
            std::clog << "i=" << i;
        }
    }
    expect(value(u128(1,0)) == value(u128(1)<<u128(64)));
    for (int i=65; i<128; ++i) {
        u64 y = u64(1)<<u64(i-64);
        if (!expect(value(u128(y,0)) == value(u128(1)<<u128(i)))) {
            std::clog << "i=" << i;
        }
        u128 x(1);
        x <<= u128(i);
        if (!expect(value(u128(y,0)) == value(x))) { std::clog << "i=" << i; }
    }
}

void test_uint128_operator_shift_right() {
    for (int i=0; i<64; ++i) {
        u64 y = u64(1)>>u64(i);
        if (!expect(value(u128(0,y)) == value(u128(1)>>u128(i)))) { std::clog << "i=" << i; }
        u128 x(1);
        x >>= u128(i);
        if (!expect(value(u128(0,y)) == value(x))) { std::clog << "i=" << i; }
    }
    expect(value(u128(0,1)) == value(u128(1,0)>>u128(64)));
    for (int i=65; i<128; ++i) {
        u64 y = u64(1)>>u64(i-64);
        if (!expect(value(u128(y,0)) == value(u128(1)>>u128(i)))) { std::clog << "i=" << i; }
        u128 x(1);
        x >>= u128(i);
        if (!expect(value(u128(y,0)) == value(x))) { std::clog << "i=" << i; }
    }
}

void test_uint128_increment() {
    u128 x;
    x = 0;
    expect(value(u128(0)) == value(x));
    expect(value(u128(1)) == value(++x));
    expect(value(u128(1)) == value(x));
    x = 0;
    expect(value(u128(0)) == value(x));
    expect(value(u128(0)) == value(x++));
    expect(value(u128(1)) == value(x));
    x = std::numeric_limits<u64>::max();
    expect(value(u128(1,0)) == value(++x));
    expect(value(u128(1,0)) == value(x));
    x = std::numeric_limits<u64>::max();
    expect(value(u128(0,std::numeric_limits<u64>::max())) == value(x++));
    expect(value(u128(1,0)) == value(x));
}

void test_uint128_decrement() {
    u128 x;
    x = 1;
    expect(value(u128(1)) == value(x));
    expect(value(u128(0)) == value(--x));
    expect(value(u128(0)) == value(x));
    x = 1;
    expect(value(u128(1)) == value(x));
    expect(value(u128(1)) == value(x--));
    expect(value(u128(0)) == value(x));
    x = u128(1,0);
    expect(value(u128(0,std::numeric_limits<u64>::max())) == value(--x));
    expect(value(u128(0,std::numeric_limits<u64>::max())) == value(x));
    x = u128(1,0);
    expect(value(u128(1,0)) == value(x--));
    expect(value(u128(0,std::numeric_limits<u64>::max())) == value(x));
}

void test_uint128_add() {
    expect(value(u128(1,0)) == value(u128(1) + u128(std::numeric_limits<u64>::max())));
    expect(value(u128(2,0)) == value(u128(1,0) + u128(1,0)));
    expect(value(u128(1,1)) == value(u128(1,0) + u128(0,1)));
    expect(value(u128(1,1)) == value(u128(0,1) + u128(1,0)));
    expect(value(u128(1,0)<<1) == value(u128(1,0) + u128(1,0)));
    expect(value(u128(0)) == value(std::numeric_limits<u128>::max()+1));
    expect(value(u128(1)) == value(std::numeric_limits<u128>::max()+2));
    expect(value(u128(1,1)) == value(u128(1,1) + std::numeric_limits<u128>::max()+1));
}

void test_uint128_add_assign() {
    u128 x;
    x = 1;
    expect(value(u128(1,0)) == value(x += u128(std::numeric_limits<u64>::max())));
    x = u128(1,0);
    expect(value(u128(2,0)) == value(x += u128(1,0)));
    x = u128(1,0);
    expect(value(u128(1,1)) == value(x += u128(0,1)));
    x = u128(0,1);
    expect(value(u128(1,1)) == value(x += u128(1,0)));
    x = u128(1,0);
    expect(value(u128(1,0)<<1) == value(x += u128(1,0)));
    x = std::numeric_limits<u128>::max();
    expect(value(u128(0)) == value(x+=1));
    x = std::numeric_limits<u128>::max();
    expect(value(u128(1)) == value(x+=2));
    x = u128(1,1);
    x += std::numeric_limits<u128>::max();
    x += 1;
    expect(value(u128(1,1)) == value(x));
}

void test_uint128_subtract() {
    expect(value(u128(std::numeric_limits<u64>::max())) == value(u128(1,0) - 1));
    expect(value(std::numeric_limits<u128>::max()) == value(u128(0) - 1));
    expect(value(-u128(1)) == value(u128(0) - 1));
    expect(value(u128(2,0)>>1) == value(u128(2,0) - u128(1,0)));
    expect(value(u128(0)) == value(u128(1,0) - u128(1,0)));
    expect(value(u128(1,1)) == value(u128(1,1) - std::numeric_limits<u128>::max()-1));
}

void test_uint128_subtract_assign() {
    u128 x;
    x = u128(1,0);
    expect(value(u128(std::numeric_limits<u64>::max())) == value(x -= 1));
    x = u128(0);
    expect(value(std::numeric_limits<u128>::max()) == value(x -= 1));
    x = u128(0);
    expect(value(-u128(1)) == value(x -= 1));
    x = u128(2,0);
    expect(value(u128(2,0)>>1) == value(x -= u128(1,0)));
    x = u128(1,0);
    expect(value(u128(0)) == value(x -= u128(1,0)));
    x = u128(1,1);
    x -= std::numeric_limits<u128>::max();
    x -= 1;
    expect(value(u128(1,1)) == value(x));
}

void test_uint128_multiply() {
    expect(value(u128(0,0)) == value(u128(1,1)*u128(0)));
    expect(value(u128(0,2)) == value(u128(0,1)*u128(2)));
    expect(value(u128(2,0)) == value(u128(1,0)*u128(2)));
    expect(value(u128(std::numeric_limits<u64>::max())+u128(std::numeric_limits<u64>::max()))
           ==
           value(u128(std::numeric_limits<u64>::max())*u128(2)));
}

void test_uint128_multiply_assign() {
    u128 x;
    x = u128(1,1);
    expect(value(u128(0,0)) == value(x *= u128(0)));
    x = u128(0,1);
    expect(value(u128(0,2)) == value(x *= u128(2)));
    x = u128(1,0);
    expect(value(u128(2,0)) == value(x *= u128(2)));
    x = u128(std::numeric_limits<u64>::max());
    expect(value(u128(std::numeric_limits<u64>::max())+u128(std::numeric_limits<u64>::max()))
           == value(x*=u128(2)));
}

void test_uint128_divide() {
    expect(value(u128(0,1)) == value(u128(1,1) / u128(1,1)));
    expect(value(u128(0,2)) == value(u128(0,2) / u128(0,1)));
    expect(value(u128(0,0)) == value(u128(0,9) % u128(0,3)));
    expect(value(u128(0,1)) == value(u128(0,10) % u128(0,3)));
    expect(value(u128(0,0)) == value(u128(0,1) / u128(0,3)));
    expect(value(u128(0,1)) == value(u128(0,1) % u128(0,3)));
    expect(value(u128(1,0)) == value(u128(2,0) / u128(0,2)));
    expect(value(u128(0,0)) == value(u128(2,0) % u128(0,2)));
}

void test_uint128_divide_assign() {
    u128 x;
    x = u128(1,1);
    expect(value(u128(0,1)) == value(x / u128(1,1)));
    x = u128(0,2);
    expect(value(u128(0,2)) == value(x / u128(0,1)));
    x = u128(0,9);
    expect(value(u128(0,0)) == value(x % u128(0,3)));
    x = u128(0,10);
    expect(value(u128(0,1)) == value(x % u128(0,3)));
    x = u128(0,1);
    expect(value(u128(0,0)) == value(x / u128(0,3)));
    x = u128(0,1);
    expect(value(u128(0,1)) == value(x % u128(0,3)));
    x = u128(2,0);
    expect(value(u128(1,0)) == value(x / u128(0,2)));
    x = u128(2,0);
    expect(value(u128(0,0)) == value(x % u128(0,2)));
}

void test_uint128_to_string() {
    expect(value("0") == value(to_string(u128(0))));
    expect(value("1") == value(to_string(u128(1))));
    expect(value("10") == value(to_string(u128(10))));
    expect(value("340282366920938463463374607431768211455") ==
           value(to_string(std::numeric_limits<u128>::max())));
}

void test_uint128_stream_insert() {
    expect(value("0") ==
           value(([](){ std::stringstream s; s << u128(0); return s.str(); })()));
    expect(value("+0") == value(([](){
        std::stringstream s;
        s.flags(std::ios::showpos);
        s << u128(0);
        return s.str();
    })()));
    expect(value("0    ") == value(([](){
        std::stringstream s;
        s.width(5);
        s.flags(std::ios::left);
        s << u128(0);
        return s.str();
    })()));
    expect(value("    0") == value(([](){
        std::stringstream s;
        s.width(5);
        s.flags(std::ios::right);
        s << u128(0);
        return s.str();
    })()));
    expect(value("   +0") == value(([](){
        std::stringstream s;
        s.width(5);
        s.flags(std::ios::right | std::ios::showpos);
        s << u128(0);
        return s.str();
    })()));
    expect(value("ff") == value(([](){
        std::stringstream s;
        s.flags(std::ios::hex);
        s << u128(255);
        return s.str();
    })()));
    expect(value("77") == value(([](){
        std::stringstream s;
        s.flags(std::ios::oct);
        s << u128(63);
        return s.str();
    })()));
    expect(value("000000+777") == value(([](){
        std::stringstream s;
        s.flags(std::ios::oct | std::ios::showpos);
        s.width(10);
        s.fill('0');
        s << u128(512-1);
        return s.str();
    })()));
}
