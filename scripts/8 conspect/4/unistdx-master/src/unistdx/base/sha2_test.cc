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

#include <unistdx/base/sha2>
#include <unistdx/test/language>

using namespace sys::test::lang;

template <class T> inline std::string
digest_to_string(const T* first, const T* last) {
    std::stringstream str;
    str << std::hex << std::setfill('0');
    while (first != last) {
        str << std::setw(sizeof(T)*2) << *first;
        ++first;
    }
    return str.str();
}

template <class T> inline std::string
digest_to_string(const T* first, std::size_t n) {
    return digest_to_string(first, first+n);
}

void test_sha2_224_empty() {
    sys::sha2_224 s;
    s.insert("", 0);
    s.finish();
    expect(value("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f") ==
           value(s.to_string()));
}

void test_sha2_256_empty() {
    sys::sha2_256 s;
    s.insert("", 0);
    s.finish();
    expect(value("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") ==
           value(s.to_string()));
}

void test_sha2_512_empty() {
    sys::sha2_512 s;
    s.insert("", 0);
    s.finish();
    expect(value("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e") ==
           value(s.to_string()));
}

void test_sha2_512_abc() {
    sys::sha2_512 s;
    s.insert("abc", 3);
    s.finish();
    expect(value("ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f") ==
           value(s.to_string()));
}

void test_sha2_512_two_blocks() {
    std::string message = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    sys::sha2_512 s;
    s.insert(message.data(), message.size());
    s.finish();
    expect(value("8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909") ==
           value(s.to_string()));
}

void test_sha2_384_empty() {
    sys::sha2_384 s;
    s.insert("", 0);
    s.finish();
    expect(value("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b") ==
           value(s.to_string()));
}

void test_sha2_512_224_empty() {
    sys::sha2_512_224 s;
    s.insert("", 0);
    s.finish();
    expect(value("6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4") ==
           value(s.to_string()));
}

void test_sha2_512_256_empty() {
    sys::sha2_512_256 s;
    s.insert("", 0);
    s.finish();
    expect(value("c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a") ==
           value(s.to_string()));
}
