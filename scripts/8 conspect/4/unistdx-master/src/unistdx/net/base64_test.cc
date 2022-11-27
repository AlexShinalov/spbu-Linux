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

#include <random>
#include <unordered_set>

#include <unistdx/base/base64>
#include <unistdx/base/log_message>
#include <unistdx/test/language>
#include <unistdx/test/properties>
#include <unistdx/test/random_string>

using namespace sys::test::lang;

using sys::base64_decode;
using sys::base64_encode;
using sys::base64_encoded_size;
using sys::base64_max_decoded_size;

typedef std::pair<std::string, std::string> pair_type;

std::vector<size_t> small_sizes{0, 1, 2, 3, 4, 22, 77, 4095, 4096, 4097};
std::vector<size_t> big_sizes{
    std::numeric_limits<size_t>::max(),
    std::numeric_limits<size_t>::max()/4u*3u
};

std::vector<pair_type> encode_pairs{
    {"", ""},
    {"a", "YQ=="},
    {"aa", "YWE="},
    {"jM", "ak0="},
    {"aaa", "YWFh"},
    {"abc", "YWJj"},
    {"aaaa", "YWFhYQ=="},
    {"abcd", "YWJjZA=="},
    // test vectors from the specification
    {"", ""},
    {"f", "Zg=="},
    {"fo", "Zm8="},
    {"foo", "Zm9v"},
    {"foob", "Zm9vYg=="},
    {"fooba", "Zm9vYmE="},
    {"foobar", "Zm9vYmFy"},
    // binary data
    {{"\0",1}, "AA=="},
    {{"\0\0",2}, "AAA="},
    {{"\0\0\0",3}, "AAAA"},
};

std::vector<pair_type> decode_pairs = encode_pairs;

void test_base64_small_sizes() {
    for (auto k : small_sizes) {
        size_t sz1 = 0, sz2 = 0;
        expect(no_throw(call([&] () {
            sz1 = base64_encoded_size(k);
            sz2 = base64_max_decoded_size(sz1);
        })));
        expect(value(sz2) >= value(k));
    }
}

void test_base64_big_sizes() {
    for (auto k : big_sizes) {
        size_t sz1 = 0, sz2 = 0;
        expect(throws<std::length_error>(call([&] () {
            sz1 = base64_encoded_size(k);
            sz2 = base64_max_decoded_size(sz1);
        })));
        expect(value(0u) == value(sz1));
        expect(value(0u) == value(sz2));
    }
}

template<class T>
void do_test_base64(size_t k, T spoil) {
    typedef std::basic_string<T> string;
    string text = test::random_string<T>(k);
    string encoded(base64_encoded_size(k), '_');
    string decoded(base64_max_decoded_size(encoded.size()), '_');
    base64_encode(text.data(), text.data() + text.size(), &encoded[0]);
    if (spoil && k > 0) {
        const size_t m = std::min(size_t(4), encoded.size());
        const size_t esize = encoded.size();
        std::unordered_set<size_t> positions;
        // first four indices
        for (size_t pos=0; pos<m; ++pos) {
            positions.insert(pos);
        }
        // last four indices
        for (size_t pos=std::max(esize, size_t(4))-4; pos<esize; ++pos) {
            positions.insert(pos);
        }
        for (size_t pos : positions) {
            std::swap(encoded[pos], spoil);
            expect(throws<std::invalid_argument>(call([&] () {
                base64_decode(
                    encoded.data(),
                    encoded.data() + encoded.size(),
                    &decoded[0]
                );
            })));
            std::swap(encoded[pos], spoil);
        }
    } else {
        size_t decoded_size = base64_decode(
            encoded.data(),
            encoded.data() + encoded.size(),
            &decoded[0]
        );
        decoded.resize(decoded_size);
        if (!expect(value(text) == value(decoded))) {
            std::clog << "encoded: " << encoded;
        }
    }
}

void test_base64_encode_decode_small_sizes() {
    for (auto k : small_sizes) {
        do_test_base64<char>(k, 0);
        do_test_base64<char>(k, '|');
        do_test_base64<char>(k, 128);
    }
}

void test_base64_encode_decode_big_sizes() {
    for (auto k : big_sizes) {
        expect(throws<std::length_error>(call([&] () { do_test_base64<char>(k, 0); })));
        expect(throws<std::length_error>(call([&] () { do_test_base64<char>(k, '|'); })));
        expect(throws<std::length_error>(call([&] () { do_test_base64<char>(k, 128); })));
    }
}

void test_base64_encode_known_pairs() {
    for (const auto& p : encode_pairs) {
        std::string result(base64_encoded_size(p.first.size()), '_');
        base64_encode(
            p.first.data(),
            p.first.data() + p.first.size(),
            &result[0]
        );
        expect(value(p.second) == value(result));
    }
}

void test_base64_decode_known_pairs() {
    for (auto p : encode_pairs) {
        std::swap(p.first, p.second);
        std::string result(base64_max_decoded_size(p.first.size()), '_');
        size_t n = base64_decode(
            p.first.data(),
            p.first.data() + p.first.size(),
            &result[0]
            );
        result.resize(n);
        expect(value(p.second) == value(result));
    }
}

void test_base64_invalid_argument() {
    expect(throws<std::invalid_argument>(call([] () { sys::base64_decode("", 1, nullptr); })));
    expect(throws<std::invalid_argument>(call([] () { sys::base64_decode("", 2, nullptr); })));
    expect(throws<std::invalid_argument>(call([] () { sys::base64_decode("", 3, nullptr); })));
    expect(throws<std::invalid_argument>(call([] () { sys::base64_decode("", 5, nullptr); })));
    expect(throws<std::invalid_argument>(call([] () { sys::base64_decode("", 6, nullptr); })));
    expect(throws<std::invalid_argument>(call([] () { sys::base64_decode("", 7, nullptr); })));
    expect(no_throw(call([] () {
        sys::base64_decode(
            static_cast<const char*>(nullptr),
            size_t(0),
            static_cast<char*>(nullptr)
        );
    })));
}

void test_base64_symmetry_encode_decode() {
    using namespace sys::test;
    auto prng = current_test->prng();
    falsify(
        [&prng] (const Argument_array<1>& params) {
            auto size = params[0];
            std::string input;
            input.reserve(size);
            std::uniform_int_distribution<char> dist_char(
                std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
            for (size_t i=0; i<size; ++i) { input += dist_char(prng); }
            std::string encoded(base64_encoded_size(input.size()), '_');
            sys::base64_encode(input.data(), input.size(), &encoded[0]);
            std::string decoded(base64_max_decoded_size(encoded.size()), '_');
            auto actual_size = sys::base64_decode(encoded.data(), encoded.size(), &decoded[0]);
            decoded.resize(actual_size);
            return expect(value(input) == value(decoded));
        },
        make_parameter<size_t>(0,4097));
}

const unsigned char base64_alphabet[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

void test_base64_symmetry_decode_encode() {
    using namespace sys::test;
    auto prng = current_test->prng();
    falsify(
        [&prng] (const Argument_array<2>& params) {
            auto size = params[0];
            auto padding = params[1];
            if (size%4 != 0) { size += 4-size%4; }
            size += 4-padding;
            std::string input;
            input.reserve(size);
            std::uniform_int_distribution<size_t> dist_index(0,63);
            for (size_t i=0; i<size; ++i) { input += base64_alphabet[dist_index(prng)]; }
            for (size_t i=0; i<padding; ++i) { input += '='; }
            std::string decoded(base64_max_decoded_size(input.size()), '_');
            auto actual_size = sys::base64_decode(input.data(), input.size(), &decoded[0]);
            decoded.resize(actual_size);
            std::string encoded(base64_encoded_size(decoded.size()), '_');
            sys::base64_encode(decoded.data(), decoded.size(), &encoded[0]);
            expect(value(input) == value(encoded));
        },
        make_parameter<size_t>(0,4097),
        make_parameter<size_t>(0,0));
}
