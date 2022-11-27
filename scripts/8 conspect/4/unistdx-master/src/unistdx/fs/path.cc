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

#include <unistdx/base/check>
#include <unistdx/fs/path>

#if defined(UNISTDX_HAVE_SYS_XATTR_H)
#include <sys/types.h>
#include <sys/xattr.h>

sys::string
sys::path_view::attribute(c_string name, file_attribute_options f) const {
    string value;
    auto get = bool(f & file_attribute_options::no_follow) ? ::lgetxattr : ::getxattr;
    auto size = get(data(), name, nullptr, 0);
    UNISTDX_CHECK(size);
    if (size == 0) { return string(); }
    while (true) {
        value.capacity(size+1);
        auto new_size = get(data(), name, value.data(), size);
        UNISTDX_CHECK(new_size);
        if (new_size == size) { break; }
        size = new_size;
    }
    value[size] = 0;
    return value;
}

void
sys::path_view::attribute(c_string name, const_string value, file_attribute_flags f1,
                     file_attribute_options f) {
    auto set = bool(f & file_attribute_options::no_follow) ? ::lsetxattr : ::setxattr;
    UNISTDX_CHECK(set(data(), name, value.data(), value.size(), int(f1)));
}

void
sys::path_view::remove_attribute(c_string name, file_attribute_options f) {
    auto remove = bool(f & file_attribute_options::no_follow) ? ::lremovexattr : ::removexattr;
    UNISTDX_CHECK(remove(data(), name));
}

sys::file_attributes sys::path_view::attributes(file_attribute_options f) const {
    auto list = bool(f & file_attribute_options::no_follow) ? ::llistxattr : ::listxattr;
    std::unique_ptr<char[]> names;
    auto size = list(data(), nullptr, 0);
    if (size == 0) { return file_attributes(); }
    UNISTDX_CHECK(size);
    while (true) {
        names.reset(new char[size]);
        auto new_size = list(data(), names.get(), size);
        UNISTDX_CHECK(new_size);
        if (new_size == size) { break; }
        size = new_size;
    }
    return file_attributes(std::move(names), size);
}
#endif
