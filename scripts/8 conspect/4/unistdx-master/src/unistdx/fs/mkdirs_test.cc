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

#include <unistdx/fs/canonical_path>
#include <unistdx/fs/file_status>
#include <unistdx/fs/mkdirs>
#include <unistdx/ipc/identity>
#include <unistdx/test/language>
#include <unistdx/test/temporary_file>

using namespace sys::test::lang;

void test_make_directories_full() {
    sys::path root(UNISTDX_TMPDIR);
    test::tmpdir tdir_h(root);
    expect(no_throw(call([&] () { sys::mkdirs(sys::path(root, "1", "2", "3")); })));
    expect(no_throw(call([&] () { sys::mkdirs(sys::path(root, "a", "b")); })));
    expect(no_throw(call([&] () { sys::mkdirs(sys::path(root, "x")); })));
    expect(no_throw(call([&] () { sys::mkdirs(sys::path(root, "x")); })));
    expect(no_throw(call([&] () { sys::mkdirs(sys::path(sys::canonical_path(root), "y")); })));
    expect(no_throw(call([&] () { sys::mkdirs(sys::path("")); })));
    expect(sys::file_status(sys::path(root, "1", "2", "3")).exists());
}

/*
void test_mkdirs_directory_permissions() {
    sys::path root(UNISTDX_TMPDIR);
    test::tmpdir tdir_h(root);
    if (sys::this_process::user() == sys::superuser()) {
        std::exit(77);
    }
    UNISTDX_CHECK(::chmod(root, 0));
    try {
        sys::mkdirs(sys::path(root, "z"));
        fail("permissions does not work");
    } catch (const sys::bad_call& err) {
        if (!expect(value(std::errc::permission_denied) == value(err.errc()))) {
            std::clog << "err=" << err.what();
        }
    }
    UNISTDX_CHECK(::chmod(root, 0755));
}
*/

void test_mkdirs_file_in_path() {
    sys::path root(UNISTDX_TMPFILE);
    test::temporary_file tmp(root);
    try {
        sys::mkdirs(sys::path(root, "x"));
        expect(false);
    } catch (const sys::bad_call& err) {
        if (!expect(value(std::errc::not_a_directory) == value(err.errc()))) {
            std::clog << "err=" << err.what();
        }
    }
    try {
        sys::mkdirs(sys::path(root, "x", "y"));
        expect(false);
    } catch (const sys::bad_call& err) {
        if (!expect(value(std::errc::not_a_directory) == value(err.errc()))) {
            std::clog << "err=" << err.what();
        }
    }
    try {
        sys::mkdirs(sys::path(root));
        expect(false);
    } catch (const sys::bad_call& err) {
        if (!expect(value(std::errc::not_a_directory) == value(err.errc()))) {
            std::clog << "err=" << err.what();
        }
    }
}
