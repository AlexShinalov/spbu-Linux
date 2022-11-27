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

#include <unistd.h>

#include <unwindx/unwind>

unw::error_category unw::unwind_category;

const char* unw::error_category::name() const noexcept {
    return "unwind";
}

std::string unw::error_category::message(int ev) const noexcept {
    return unw_strerror(ev);
}

void unw::backtrace(int fd) noexcept {
    char symbol[4096];
    unw_cursor_t cursor{};
    unw_context_t context{};
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);
    while (unw_step(&cursor) > 0) {
        unw_word_t ip = 0;
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        if (ip == 0) { break; }
        unw_word_t offset = 0;
        for (auto& ch : symbol) { ch = 0; }
        if (unw_get_proc_name(&cursor, symbol, sizeof(symbol), &offset) == 0) {
            int length = 0;
            while (length != sizeof(symbol) && symbol[length] != 0) { ++length; }
            ::write(fd, symbol, length);
            ::write(fd, "\n", 1);
        } else {
            ::write(fd, "-\n", 2);
        }
    }

}
