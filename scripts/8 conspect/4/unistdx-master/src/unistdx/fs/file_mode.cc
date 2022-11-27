/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2020 Ivan Gankevich

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

#include <unistdx/fs/file_mode>

#include <ostream>

std::ostream&
sys::operator<<(std::ostream& out, const file_mode& rhs) {
    const mode_type s = rhs.special();
    const mode_type usr = rhs.user();
    const mode_type grp = rhs.group();
    const mode_type oth = rhs.other();
    const char bits[] = {
        ((s & sys::file_mode::setuid) ? 'u' : '-'),
        ((s & sys::file_mode::setgid) ? 'g' : '-'),
        ((s & sys::file_mode::sticky) ? 't' : '-'),
        ((usr & sys::file_mode::user_r) ? 'r' : '-'),
        ((usr & sys::file_mode::user_w) ? 'w' : '-'),
        ((usr & sys::file_mode::user_x) ? 'x' : '-'),
        ((grp & sys::file_mode::group_r) ? 'r' : '-'),
        ((grp & sys::file_mode::group_w) ? 'w' : '-'),
        ((grp & sys::file_mode::group_x) ? 'x' : '-'),
        ((oth & sys::file_mode::other_r) ? 'r' : '-'),
        ((oth & sys::file_mode::other_w) ? 'w' : '-'),
        ((oth & sys::file_mode::other_x) ? 'x' : '-'),
        0
    };
    return out << bits;
}
