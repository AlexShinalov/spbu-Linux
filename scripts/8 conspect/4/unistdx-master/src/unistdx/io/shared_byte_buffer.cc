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

#include <iomanip>
#include <sstream>

#include <unistdx/base/contracts>
#include <unistdx/io/shared_byte_buffer>

auto sys::memory_file_allocator::reallocate(value_type* data, size_type old_size,
                                            size_type new_size) -> value_type* {
    this->_file_descriptor.truncate(new_size);
    value_type* result = nullptr;
    if (new_size == 0) {
        if (data && old_size) { sys::check(::munmap(data, old_size)); }
    } else if (old_size == 0) {
        result = static_cast<value_type*>(
            check(::mmap(nullptr, new_size*sizeof(value_type),
                         PROT_READ | PROT_WRITE, MAP_SHARED,
                         this->_file_descriptor.get(), 0),
                  MAP_FAILED));
    } else {
        result = static_cast<value_type*>(
            check(::mremap(data, old_size, new_size, MREMAP_MAYMOVE), MAP_FAILED));
    }
    return result;
}

void sys::shared_byte_buffer::representation::copy_to_environment(c_string name) const {
    std::stringstream tmp;
    tmp << std::hex << parent.address() << ' ' << std::dec << parent.size_in_bytes() << ' ';
    tmp << std::hex << child.address() << ' ' << std::dec << child.size_in_bytes() << ' ';
    tmp << data;
    check(::setenv(name, tmp.str().data(), 1));
}

sys::shared_byte_buffer::representation::representation(c_string name) {
    auto value = std::getenv(name);
    if (!value) {
        throw std::invalid_argument("failed to get file descriptors from the environment");
    }
    uintptr_t parent_address = 0, child_address = 0;
    size_type parent_size = 0, child_size = 0;
    std::stringstream tmp(value);
    tmp >> std::hex >> parent_address >> std::dec >> parent_size;
    tmp >> std::hex >> child_address >> std::dec >> child_size;
    tmp >> data;
    UNISTDX_ASSERTION(parent_address%alignof(parent_page) == 0);
    UNISTDX_ASSERTION(child_address%alignof(child_page) == 0);
    if (!(sizeof(parent_page) <= parent_size)) {
        throw std::invalid_argument("wrong parent page size");
    }
    if (!(sizeof(child_page) <= child_size)) {
        throw std::invalid_argument("wrong child page size");
    }
    parent.data = reinterpret_cast<parent_page*>(parent_address);
    parent.size = 1;
    child.data = reinterpret_cast<child_page*>(child_address);
    child.size = 1;
}
