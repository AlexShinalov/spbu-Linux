/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <algorithm>
#include <set>

#include <unistdx/fs/canonical_path>
#include <unistdx/fs/idirectory>
#include <unistdx/fs/idirtree>
#include <unistdx/fs/odirectory>
#include <unistdx/fs/odirtree>
#include <unistdx/fs/path>
#include <unistdx/test/base>
#include <unistdx/test/language>
#include <unistdx/test/temporary_file>

using namespace sys::test::lang;

void test_idirtree_open_close() {
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    sys::idirtree tree;
    test::test_open_close(tree, tdir);
}

void test_idirtree_current_dir() {
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    sys::idirtree tree(tdir);
    expect(value(tree.current_dir()) == value(static_cast<sys::path_view>(tdir)));
}

void test_idirtree_iterator() {
    std::vector<std::string> files {"a", "b", "c"};
    std::vector<std::string> files_d {"e", "f", "g"};
    std::vector<std::string> files_h {"i", "j", "k", "l"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    test::tmpdir tdir_d(
        sys::path(UNISTDX_TMPDIR, "d"),
        files_d.begin(),
        files_d.end()
    );
    test::tmpdir tdir_h(
        sys::path(UNISTDX_TMPDIR, "h"),
        files_h.begin(),
        files_h.end()
    );
    std::vector<std::string> all_files;
    std::copy(files.begin(), files.end(), std::back_inserter(all_files));
    std::copy(files_d.begin(), files_d.end(), std::back_inserter(all_files));
    std::copy(files_h.begin(), files_h.end(), std::back_inserter(all_files));
    all_files.emplace_back("d");
    all_files.emplace_back("h");
    typedef sys::idirtree_iterator<sys::directory_entry> iterator;
    test::test_file_count<sys::idirtree,iterator>(tdir, all_files);
    test::test_file_list<sys::idirtree,iterator>(tdir, all_files);
}

void test_file_stat_exists() {
    sys::canonical_path cwd(".");
    sys::path non_existent_file(cwd, "asdasdasd");
    sys::file_status stat;
    try {
        stat.update(non_existent_file);
    } catch (const sys::bad_call& err) {
        expect(value(ENOENT) == value(err.code().value()));
    }
    expect(!stat.exists());
}

void test_get_file_type_direntry() {
    test::temporary_file tmp(UNISTDX_TMPFILE);
    std::ofstream(tmp.path()) << "hello world";
    sys::path cwd(".");
    sys::idirectory dir(cwd);
    sys::idirectory_iterator<sys::directory_entry> end;
    auto result =
        std::find_if(
            sys::idirectory_iterator<sys::directory_entry>(dir),
            end,
            [&] (const sys::directory_entry& rhs) {
                return rhs.name() == tmp.path();
            }
        );
    expect(value(result) != value(end));
    expect(value(sys::file_type::regular) == value(sys::get_file_type(cwd, *result)));
}

void test_get_file_type_pathentry() {
    test::temporary_file tmp(UNISTDX_TMPFILE);
    std::ofstream(tmp.path()) << "hello world";
    sys::path cwd(".");
    sys::idirtree dir(cwd);
    sys::idirtree_iterator<sys::directory_entry> end;
    auto result =
        std::find_if(
            sys::idirtree_iterator<sys::directory_entry>(dir),
            end,
            [&] (const sys::directory_entry& rhs) {
                return rhs.name() == tmp.path();
            }
        );
    expect(value(result) != value(end));
    expect(value(sys::file_type::regular) ==
           value(sys::get_file_type(dir.current_dir(), *result)));
}

struct idirtree_test_param {
    std::string src;
};

std::ostream& operator<<(std::ostream& out, const idirtree_test_param& rhs) {
    return out << rhs.src;
}

std::vector<idirtree_test_param> all_params {
    {"dirtree-copy-in"},
    {"dirtree-copy-in/"},
    {"dirtree-copy-in//"},
};

void test_idirtree_test_insert() {
    for (const auto& param : all_params) {
        sys::path src(param.src);
        std::vector<std::string> files1 {"a", "b", "c"};
        std::vector<std::string> files2 {"d", "e", "f"};
        test::tmpdir dir1(src, files1.begin(), files1.end());
        test::tmpdir dir2(sys::path(src, "next"), files2.begin(), files2.end());
        test::tmpdir dir3("dirtree-copy-out");
        std::set<sys::path> expected;
        expected.emplace("a");
        expected.emplace("b");
        expected.emplace("c");
        expected.emplace("next");
        expected.emplace("next/d");
        expected.emplace("next/e");
        expected.emplace("next/f");
        {
            sys::idirtree idir(dir1);
            sys::odirtree odir(dir3);
            odir.settransform(sys::copy_recursively{dir1, dir3});
            odir << idir;
            /*
            std::copy(
                sys::idirtree_iterator<sys::directory_entry>(idir),
                sys::idirtree_iterator<sys::directory_entry>(),
                sys::odirtree_iterator<sys::directory_entry>(odir)
            );
            */
        }
        std::set<sys::path> actual;
        {
            sys::idirtree idir(dir3);
            std::transform(
                sys::idirtree_iterator<sys::directory_entry>(idir),
                sys::idirtree_iterator<sys::directory_entry>(),
                std::inserter(actual, actual.begin()),
                [&] (const sys::directory_entry& rhs) {
                    sys::path p(idir.current_dir(), rhs.name());
                    if (p.find(dir3.name()) == 0) {
                        p = p.substr(dir3.name().size()+1);
                    }
                    return p;
                }
            );
        }
        expect(value(expected) == value(actual));
    }
}
