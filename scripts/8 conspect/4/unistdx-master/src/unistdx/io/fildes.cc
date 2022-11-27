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

#include <unistdx/base/make_object>
#include <unistdx/fs/file_status>
#include <unistdx/io/fildes>
#include <unistdx/net/socket>

sys::fildes&
sys::fildes::operator=(const fildes& rhs) {
    if (*this) {
        this->_fd = ::dup2(rhs._fd, this->_fd);
    } else {
        this->_fd = ::dup(rhs._fd);
    }
    UNISTDX_CHECK(this->_fd);
    return *this;
}

#if defined(UNISTDX_HAVE_SYS_XATTR_H)
std::string
sys::fildes::attribute(c_string name) const {
    std::string value;
    auto size = ::fgetxattr(fd(), name, nullptr, 0);
    UNISTDX_CHECK(size);
    if (size == 0) { return std::string(); }
    while (true) {
        value.resize(size);
        auto new_size = ::fgetxattr(fd(), name, &value[0], size);
        UNISTDX_CHECK(new_size);
        if (new_size == size) { break; }
        size = new_size;
    }
    return value;
}

void
sys::fildes::attribute(c_string name, const_string value, file_attribute_flags f1) {
    UNISTDX_CHECK(::fsetxattr(fd(), name, value.data(), value.size(), int(f1)));
}

void
sys::fildes::remove_attribute(c_string name) {
    UNISTDX_CHECK(::fremovexattr(fd(), name));
}

sys::file_attributes
sys::fildes::attributes() const {
    std::unique_ptr<char[]> names;
    auto size = ::flistxattr(fd(), nullptr, 0);
    if (size == 0) { return file_attributes(); }
    UNISTDX_CHECK(size);
    while (true) {
        names.reset(new char[size]);
        auto new_size = ::flistxattr(fd(), names.get(), size);
        UNISTDX_CHECK(new_size);
        if (new_size == size) { break; }
        size = new_size;
    }
    return file_attributes(std::move(names), size);
}
#endif

bool sys::file_descriptor_view::is_socket() const noexcept {
    file_status status;
    if (::fstat(get(), &status) != -1) {
        return status.is_socket();
    }
    int value = 0;
    socket_length_type n = sizeof(value);
    if (::getsockopt(get(), SOL_SOCKET, SO_TYPE, &value, &n) == 0) {
        return true;
    }
    return false;
}
