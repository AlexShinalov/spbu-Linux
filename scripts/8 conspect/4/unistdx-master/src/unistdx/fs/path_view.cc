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

#include <unistdx/fs/path_view>

#if defined(UNISTDX_HAVE_MOUNT)
void sys::path_view::set_root(const char* old_name) {
    path_view("/").flags(mount_flags::make_private | mount_flags::recursive);
    path_view(data()).make_mount_point(data(), mount_flags::bind);
    std::string old;
    old += data();
    old += old_name;
    if (::mkdir(old.data(), 0700) == -1 && errno != EEXIST) { throw bad_call(); }
    check(call(calls::pivot_root, data(), old.data()));
    path_view("/").set_working_directory();
    path_view(old_name).unmount(unmount_flags::detach);
    path_view(old_name).remove_directory();
}
#endif
