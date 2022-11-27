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

#include <unistdx/io/event_file_descriptor>
#include <unistdx/io/shared_byte_buffer>
#include <unistdx/ipc/process>
#include <unistdx/test/language>
#include <unistdx/test/random_string>

using namespace sys::test::lang;

void test_shared_byte_buffer_grow() {
    auto parent = sys::shared_byte_buffer::make_parent_page();
    expect(value(parent.get()) != value(nullptr));
    expect(value(parent.size()) != value(0u));
    sys::shared_byte_buffer buffer{parent.view(), 4096};
    expect(value(1) == value(buffer.parent()->mutex.value()));
    expect(value(0) == value(buffer.parent()->semaphore.value()));
    expect(value(1) == value(buffer.child()->mutex.value()));
    expect(value(0) == value(buffer.child()->semaphore.value()));
    buffer.get_representation().copy_to_environment("FDS");
    sys::event_file_descriptor notifier(
        0, sys::event_file_descriptor::flag::close_on_exec);
    sys::process child{[&] () -> int {
        sys::shared_byte_buffer::representation fds("FDS");
        auto parent = sys::shared_byte_buffer::make_parent_page(fds.parent);
        sys::shared_byte_buffer buffer{fds};
        //sys::shared_byte_buffer buffer{parent, fds};
        auto& mtx = buffer.child()->mutex;
        auto& cv = buffer.child()->semaphore;
        notifier.write(1);
        std::clog << "child wait" << std::endl;
        cv.wait();
        std::clog << "child lock" << std::endl;
        std::lock_guard<sys::futex> lock(mtx);
        buffer.copy_in();
        std::clog << "buffer.size()=" << buffer.size() << std::endl;
        return buffer.size() == 2*4096 ? 0 : 1;
    }};
    {
        notifier.read();
        auto& mtx = buffer.child()->mutex;
        auto& cv = buffer.child()->semaphore;
        std::clog << "parent lock" << std::endl;
        mtx.lock();
        buffer.grow();
        buffer.copy_out();
        std::clog << "parent unlock" << std::endl;
        mtx.unlock();
        std::clog << "parent notify" << std::endl;
        cv.notify_one();
    }
    auto status = child.wait();
    expect(value(buffer.size()) == value(2u*4096u));
    expect(value(status.exited()) == value(true));
    expect(value(status.exit_code()) == value(0));
}

struct datum {
    int i;
    std::string s;
    void write(sys::byte_buffer& out) const {
        out.write(i);
        out.write(s);
    }
    void read(sys::byte_buffer& in) {
        in.read(i);
        in.read(s);
    }
    inline bool operator==(const datum& rhs) const noexcept {
        return i == rhs.i && s == rhs.s;
    }
    inline bool operator!=(const datum& rhs) const noexcept {
        return !this->operator==(rhs);
    }
    friend std::ostream& operator<<(std::ostream& out, const datum& rhs) {
        return out << "i=" << rhs.i << ",s=" << rhs.s;
    }
};

void reader_main(sys::shared_byte_buffer& buffer, std::vector<std::vector<datum>>& data) {
    auto* child = buffer.child();
    auto* parent = buffer.parent();
    for (size_t i=0; i<data.size(); ++i) {
        tell(std::clog, "[child] child wait");
        child->semaphore.wait();
        tell(std::clog, "[child] child lock");
        child->mutex.lock();
        buffer.copy_in();
        tell(std::clog, "[child] child buffer size = ", buffer.size());
        size_t new_data_size = 0;
        buffer.flip();
        buffer.read(new_data_size);
        expect(value(new_data_size) == value(data[i].size()));
        std::vector<datum> new_data;
        new_data.reserve(new_data_size);
        for (size_t i=0; i<new_data_size; ++i) {
            new_data.emplace_back();
            new_data.back().read(buffer);
        }
        expect(value(new_data) == value(data[i]));
        buffer.compact();
        buffer.copy_out();
        expect(value(buffer.position()) == value(0u));
        expect(value(buffer.limit()) == value(buffer.size()));
        tell(std::clog, "[child] child unlock");
        child->mutex.unlock();
        tell(std::clog, "[child] parent notify");
        parent->semaphore.notify_one();
    }
}

void writer_main(sys::shared_byte_buffer& buffer, std::vector<std::vector<datum>>& data) {
    auto* child = buffer.child();
    auto* parent = buffer.parent();
    for (size_t i=0; i<data.size(); ++i) {
        {
            tell(std::clog, "[parent] child lock");
            auto g = buffer.guard();
            expect(value(buffer.position()) == value(0u));
            expect(value(buffer.limit()) == value(buffer.size()));
            buffer.write(data[i].size());
            for (const auto& x : data[i]) { x.write(buffer); }
            tell(std::clog, "[parent] child unlock");
        }
        tell(std::clog, "[parent] child notify");
        child->semaphore.notify_one();
        tell(std::clog, "[parent] parent wait");
        parent->semaphore.wait();
    }
}

void test_shared_byte_buffer__parent_writes_child_reads() {
    const size_t num_arrays = 11;
    const size_t data_size = 123;
    std::vector<std::vector<datum>> data;
    std::uniform_int_distribution<int> dist(
        std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    std::uniform_int_distribution<int> dist2(0, 100);
    for (size_t i=0; i<num_arrays; ++i) {
        data.emplace_back();
        auto& x = data.back();
        x.reserve(data_size);
        for (size_t j=0; j<data_size; ++j) {
            x.emplace_back(datum{dist(test::rng), test::random_string<>(dist2(test::rng))});
        }
    }
    auto parent = sys::shared_byte_buffer::make_parent_page();
    sys::shared_byte_buffer buffer{parent.view(), 4096};
    buffer.get_representation().copy_to_environment("FDS");
    sys::event_file_descriptor notifier(
        0, sys::event_file_descriptor::flag::close_on_exec);
    sys::process child{[&] () -> int {
        sys::shared_byte_buffer::representation fds("FDS");
        auto parent_page = sys::shared_byte_buffer::make_parent_page(fds.parent);
        sys::shared_byte_buffer buffer{fds};
        notifier.write(1);
        //sys::shared_byte_buffer buffer{parent, fds};
        reader_main(buffer, data);
        using namespace sys::test;
        return current_test->status() == Test::Status::Success ? 0 : 1;
    }};
    notifier.read();
    writer_main(buffer, data);
    auto status = child.wait();
    //expect(value(buffer.size()) == value(2u*4096u));
    expect(value(status.exited()) == value(true));
    expect(value(status.exit_code()) == value(0));
}

void test_shared_byte_buffer__child_writes_parent_reads() {
    const size_t num_arrays = 11;
    const size_t data_size = 123;
    std::vector<std::vector<datum>> data;
    std::uniform_int_distribution<int> dist(
        std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    std::uniform_int_distribution<int> dist2(0, 100);
    for (size_t i=0; i<num_arrays; ++i) {
        data.emplace_back();
        auto& x = data.back();
        x.reserve(data_size);
        for (size_t j=0; j<data_size; ++j) {
            x.emplace_back(datum{dist(test::rng), test::random_string<>(dist2(test::rng))});
        }
    }
    auto parent = sys::shared_byte_buffer::make_parent_page();
    sys::shared_byte_buffer buffer{parent.view(), 4096};
    buffer.get_representation().copy_to_environment("FDS");
    sys::event_file_descriptor notifier(
        0, sys::event_file_descriptor::flag::close_on_exec);
    sys::process child{[&] () -> int {
        sys::shared_byte_buffer::representation fds("FDS");
        auto parent_page = sys::shared_byte_buffer::make_parent_page(fds.parent);
        sys::shared_byte_buffer buffer{fds};
        //sys::shared_byte_buffer buffer{parent, fds};
        notifier.write(1);
        writer_main(buffer, data);
        using namespace sys::test;
        return current_test->status() == Test::Status::Success ? 0 : 1;
    }};
    notifier.read();
    reader_main(buffer, data);
    auto status = child.wait();
    //expect(value(buffer.size()) == value(2u*4096u));
    expect(value(status.exited()) == value(true));
    expect(value(status.exit_code()) == value(0));
}
