/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2019, 2020, 2021 Ivan Gankevich

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
#include <string>
#include <tuple>
#include <vector>

#include <unistdx/base/sha1>
#include <unistdx/bits/macros>
#include <unistdx/net/byte_order>
#include <unistdx/net/bytes>
#include <unistdx/test/language>

using namespace sys::test::lang;
using sys::u32;

const std::string SHA_OF_ONE_MILLION_OF_A = "34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f";
const std::string SHA_OF_64_OF_A = "0098ba82 4b5c1642 7bd7a112 2a5a442a 25ec644d";

std::string sha1_digest_to_string(const u32* first, const u32* last) {
    std::stringstream str;
    str << std::hex << std::setfill('0');
    std::for_each(first, last, [&str] (u32 n) {
        str << std::setw(8) << n << ' ';
    });
    std::string output = str.str();
    output.pop_back(); // remove space character
    return output;
}

std::string sha1_digest_to_string(const std::vector<u32>& result) {
    return sha1_digest_to_string(result.data(), result.data() + result.size());
}

std::string sha1_digest_to_string(
    const unsigned char* first,
    const unsigned char* last
) {
    expect(value(sys::sha1::digest_bytes_length()) == value(last-first));
    sys::bytes<u32> arr[sys::sha1::digest_length()];
    u32 arr2[sys::sha1::digest_length()];
    for (int i=0; i<sys::sha1::digest_length(); ++i) {
        std::copy(first + i*4, first + (i+1)*4, arr[i].begin());
//		arr[i].to_host_format();
        arr2[i] = arr[i].value();
    }
    return sha1_digest_to_string(arr2, arr2 + sys::sha1::digest_length());
}

std::string sha1_digest_to_string(const char* first, const char* last) {
    return sha1_digest_to_string(
        reinterpret_cast<const unsigned char*>(first),
        reinterpret_cast<const unsigned char*>(last)
    );
}

std::string sha1_digest_to_string(const std::vector<unsigned char>& result) {
    return sha1_digest_to_string(result.data(), result.data() + result.size());
}

std::string sha1_digest_to_string(const std::vector<char>& result) {
    return sha1_digest_to_string(result.data(), result.data() + result.size());
}

arguments<std::string,std::string> args_sha1_known_hashes = {
    {
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "c1c8bbdc 22796e28 c0e15163 d20899b6 5621d65a"
    },
    {"",    "da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709"},
    {"sha", "d8f45903 20e1343a 915b6394 170650a8 f35d6926"},
    {"Sha", "ba79baeb 9f10896a 46ae7471 5271b7f5 86e74640"},
    {
        "The quick brown fox jumps over the lazy dog",
        "2fd4e1c6 7a2d28fc ed849ee1 bb76e739 1b93eb12"
    },
    {
        "The quick brown fox jumps over the lazy cog",
        "de9f2c7f d25e1b3a fad3e85a 0bd17d9b 100db4b3"
    },
    {"abc", "a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"},
    {
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1"
    },
};

void test_sha1_known_hashes(std::string* input, std::string* expected_output) {
    std::vector<u32> result(5);
    sys::sha1 sha;
    sha.put(input->data(), input->size());
    sha.compute();
    sha.digest(result.data());
    std::string output = sha1_digest_to_string(result);
    expect(value(*expected_output) == value(output));
}

void test_sha1_one_million_of_a() {
    sys::sha1 sha;
    std::string a = "aaaaaaaaaa";
    for (int i=0; i<100000; i++) {
        sha.put(a.data(), a.size());
    }
    std::vector<u32> result(5);
    sha.compute();
    sha.digest(result.data());
    std::string output = sha1_digest_to_string(result);
    if (!expect(value(SHA_OF_ONE_MILLION_OF_A) == value(output))) {
        std::clog << "SHA of one million of 'a' failed\n";
    }
}

void test_sha1_big_inputs() {
    sys::sha1 sha;
    std::string tmp;
    expect(throws(call([&] () { sha.put(tmp.data(), std::numeric_limits<size_t>::max()); })));
    sha.reset();
    sha.put("a", 1);
    expect(throws(call([&] () { sha.put(tmp.data(), std::numeric_limits<size_t>::max()/8-1); })));
}

void test_sha1_repeating_compute() {
    sys::sha1 sha;
    std::vector<char> a(64, 'a');
    sha.put(a.data(), a.size());
    sha.compute();
    expect(value(a.size()*8) == value(sha.length()));
    sha.compute();
    std::vector<u32> result(5);
    sha.digest(result.data());
    std::string output = sha1_digest_to_string(result);
    if (!expect(value(SHA_OF_64_OF_A) == value(output))) {
        std::clog << "SHA of 64 of 'a' failed\n";
    }
}

void test_sha1_put() {
    std::vector<char> a(64, 'a');
    sys::sha1 sha;
    sha.put(a.data(), a.size()/2);
    sha.put(a.data() + a.size()/2, a.data() + a.size());
    sha.compute();
    {
        std::vector<u32> result(5);
        sha.digest(result.data());
        expect(value(SHA_OF_64_OF_A) == value(sha1_digest_to_string(result)));
    }
    expect(value(SHA_OF_64_OF_A) ==
           value(sha1_digest_to_string(sha.digest(), sha.digest() + 5)));
    {
        std::vector<unsigned char> result(sys::sha1::digest_bytes_length());
        sha.digest(result.data());
        expect(value(SHA_OF_64_OF_A) == value(sha1_digest_to_string(result)));
    }
    expect(value(SHA_OF_64_OF_A) ==
           value(sha1_digest_to_string(sha.digest_bytes(), sha.digest_bytes() + 20)));
    {
        std::vector<char> result(sys::sha1::digest_bytes_length());
        sha.digest(result.data());
        expect(value(SHA_OF_64_OF_A) == value(sha1_digest_to_string(result)));
    }
    expect(value(SHA_OF_64_OF_A) ==
           value(sha1_digest_to_string(sha.digest_chars(), sha.digest_chars() + 20)));
}
