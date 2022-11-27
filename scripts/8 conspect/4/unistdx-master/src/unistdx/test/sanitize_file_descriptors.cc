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

#include <iomanip>
#include <ostream>
#include <sstream>

#include <unistdx/bits/for_each_file_descriptor>
#include <unistdx/fs/idirectory>
#include <unistdx/ipc/process>
#include <unistdx/test/sanitize_file_descriptors>

namespace {

    template <class Callback>
    inline void for_each_open_file_descriptor(Callback call) {
        char path[100];
        std::sprintf(path, "/proc/%d/fd", sys::this_process::id());
        sys::idirectory fds{sys::path(path)};
        char target[4096];
        for (const auto& entry : fds) {
            if (std::atoi(entry.name()) == fds.fd()) { continue; }
            auto size = ::readlinkat(fds.fd(), entry.name(), target, sizeof(target));
            if (size_t(size) >= sizeof(target)) {
                target[sizeof(target)-1] = 0;
            } else if (size >= 0) {
                target[size] = 0;
            }
            call(entry.name(), target);
        }
    }

}

void sys::test::print_file_descriptors(std::ostream& out) {
    for_each_open_file_descriptor([&out] (const char* name, const char* target) {
        out << std::setw(20) << std::right << name;
        out << "    ";
        out << std::setw(20) << std::left << target;
        out << '\n';
    });
}

void sys::test::sanitize_file_descriptors(bool ignore_standard, bool ignore_urandom) {
    std::stringstream out;
    bool found = false;
    for_each_open_file_descriptor(
        [&out,&found,ignore_standard,ignore_urandom] (const char* name, const char* target_in) {
            sys::fd_type fd = std::atoi(name);
            if (ignore_standard && fd >= 0 && fd <= 2) { return; }
            std::string target(target_in);
            if (ignore_urandom && target == "/dev/urandom") { return; }
            out << std::setw(20) << std::right << name;
            out << "    ";
            out << std::setw(20) << std::left << target;
            out << '\n';
            found = true;
        });
    if (found) { throw std::runtime_error("Found opened file descriptors:\n" + out.str()); }
}

sys::test::file_descriptor_sanitizer::file_descriptor_sanitizer() {
    record(this->_file_descriptors);
}

sys::test::file_descriptor_sanitizer::~file_descriptor_sanitizer() noexcept {
    std::set<entry> new_file_descritors;
    record(new_file_descritors);
    // set difference
    for (const auto& old_entry: this->_file_descriptors) {
        new_file_descritors.erase(old_entry);
    }
    if (!new_file_descritors.empty()) {
        std::stringstream out;
        out << std::setw(10) << std::right << "number";
        out << "    ";
        out << std::setw(20) << std::left << "target";
        out << '\n';
        for (const auto& entry : new_file_descritors) {
            out << std::setw(10) << std::right << std::get<0>(entry);
            out << "    ";
            out << std::setw(20) << std::left << std::get<1>(entry);
            out << '\n';
        }
        throw std::runtime_error("Found opened file descriptors:\n" + out.str());
    }
}

void sys::test::file_descriptor_sanitizer::record(std::set<entry>& entries) {
    for_each_open_file_descriptor([&] (const char* name, const char* target) {
        entries.emplace(std::atoi(name), target);
    });
}
