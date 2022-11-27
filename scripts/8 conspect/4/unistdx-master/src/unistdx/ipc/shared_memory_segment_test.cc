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

#include <unistdx/ipc/process>
#include <unistdx/ipc/shared_memory_segment>
#include <unistdx/ipc/shmembuf>
#include <unistdx/system/resource>

#include <functional>
#include <limits>
#include <mutex>
#include <random>

#include <unistdx/test/exception>
#include <unistdx/test/language>

using namespace sys::test::lang;

template <class T>
void test_shared_memory_segment() {
    typedef typename sys::shared_memory_segment<T>::size_type size_type;
    const size_type SHMEM_SIZE = 512;
    sys::shared_memory_segment<T> mem1(0666, SHMEM_SIZE);
    sys::shared_memory_segment<T> mem2(mem1.id());
    // check some invariants
    expect(value(mem1.size()) >= value(SHMEM_SIZE));
    expect(value(mem2.size()) >= value(SHMEM_SIZE));
    expect(value(mem1.size()) == value(mem2.size()));
    expect(mem1.owner());
    expect(!mem2.owner());
    if (!expect(static_cast<bool>(mem1))) {
        std::clog << "ptr=" << ((void*)mem1.ptr()) << ",id=" << mem1.id();
    }
    if (!expect(static_cast<bool>(mem2))) {
        std::clog << "ptr=" << ((void*)mem2.ptr()) << ",id=" << mem2.id();
    }
    expect(!value(!mem1));
    expect(!value(!mem2));
    expect(value(nullptr) != value(mem1.begin()));
    expect(value(nullptr) != value(mem1.end()));
    expect(value(nullptr) != value(mem2.begin()));
    expect(value(nullptr) != value(mem2.end()));
    expect(value(mem1.size()) == value(size_t(std::distance(mem1.begin(), mem1.end()))));
    expect(value(mem2.size()) == value(size_t(std::distance(mem2.begin(), mem2.end()))));
    std::default_random_engine rng;
    std::uniform_int_distribution<T> dist('a', 'z');
    std::generate(mem1.begin(), mem1.end(), std::bind(dist, rng));
    expect(value(mem1) == value(mem2));
    // close multiple times
    mem1.close();
    mem2.close();
    mem1.close();
    mem2.close();
    expect(!static_cast<bool>(mem1));
    expect(!static_cast<bool>(mem2));
}

void test_shared_memory_segment() {
    test_shared_memory_segment<char>();
    test_shared_memory_segment<unsigned char>();
}

template <class T>
void do_test_shmembuf_producer_consumer() {
    typedef typename sys::shared_memory_segment<T> shmem;
    typedef typename sys::basic_shmembuf<T> shmembuf;
    typedef std::lock_guard<shmembuf> shmembuf_guard;
    shmembuf buf1(shmem(0600, sys::page_size()*4));
    // generated random data
    const size_t ninputs = 12;
    std::default_random_engine rng;
    std::uniform_int_distribution<T> dist('a', 'z');
    std::vector<std::vector<T>> inputs;
    for (size_t i=0; i<ninputs; ++i) {
        const size_t size = 2u << i;
        inputs.emplace_back(size);
        std::generate(
            inputs.back().begin(),
            inputs.back().end(),
            std::bind(dist, rng)
        );
    }
    sys::process consumer([&] () {
        shmembuf buf2(shmem(buf1.mem().id()));
        std::clog << "buf2.mem()=" << buf2.mem() << std::endl;
        bool success = true;
        for (size_t i=0; i<ninputs; ++i) {
            const std::vector<T>& input = inputs[i];
            std::vector<T> output(input.size());
            {
                shmembuf_guard lock(buf2);
                buf2.sgetn(output.data(), output.size());
            }
            expect(value(input) == value(output));
            if (input != output) {
                success = false;
            }
        }
        return success ? EXIT_SUCCESS : EXIT_FAILURE;
    });
    for (size_t i=0; i<ninputs; ++i) {
        const std::vector<T>& input = inputs[i];
        shmembuf_guard lock(buf1);
        buf1.sputn(input.data(), input.size());
    }
    sys::process_status status = consumer.wait();
    expect(value(status.exited()) && value(status.exit_code()) == value(EXIT_SUCCESS));
}

void test_shmembuf_producer_consumer() {
    do_test_shmembuf_producer_consumer<char>();
    do_test_shmembuf_producer_consumer<unsigned char>();
}

void test_shmembuf_errors() {
    sys::size_type page_size =  sys::page_size();
    sys::shmembuf buf(sys::shared_memory_segment<char>(0600, page_size));
    std::vector<char> tmp(page_size*2);
    UNISTDX_EXPECT_ERROR(
        std::errc::not_enough_memory,
        buf.sputn(tmp.data(), tmp.size())
    );
}
