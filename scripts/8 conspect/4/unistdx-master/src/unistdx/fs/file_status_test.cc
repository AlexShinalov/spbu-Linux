/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2020 Ivan Gankevich

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

#include <unistdx/fs/file_status>

#include <unistdx/test/exception>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void test_file_status_errors() {
    UNISTDX_EXPECT_ERROR(
        std::errc::no_such_file_or_directory,
        sys::file_status("non-existent-file")
    );
    sys::file_status st;
    UNISTDX_EXPECT_ERROR(
        std::errc::no_such_file_or_directory,
        st.update("non-existent-file")
    );
}

void test_file_status_print() {
    test::stream_insert_starts_with("d", sys::file_status("src"));
}

void test_file_status_members() {
    sys::file_status st("src");
    expect(st.is_directory());
    expect(!st.is_regular());
    expect(!st.is_block_device());
    expect(!st.is_character_device());
    expect(!st.is_socket());
    expect(!st.is_pipe());
    expect(!st.is_symbolic_link());
}
