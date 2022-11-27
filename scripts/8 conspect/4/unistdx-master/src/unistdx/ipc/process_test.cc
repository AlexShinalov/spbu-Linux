/*
UNISTDX — C++ library for Linux system calls.
© 2016, 2017, 2018, 2020, 2021 Ivan Gankevich

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

#include <bitset>
#include <thread>

#include <unistdx/base/log_message>
#include <unistdx/io/pipe>
#include <unistdx/ipc/argstream>
#include <unistdx/ipc/execute>
#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>
#include <unistdx/test/config>
#include <unistdx/test/language>
#include <unistdx/test/operator>
#include <unistdx/test/properties>
#include <valgrind/config>

using namespace sys::test::lang;

void test_process_basic() {
    UNISTDX_SKIP_IF_RUNNING_ON_VALGRIND();
    sys::pid_type pid = sys::this_process::id();
    sys::process child {
        [pid] () {
            sys::pid_type parent_pid = sys::this_process::parent_id();
            sys::pid_type child_pid = sys::this_process::id();
            int ret = 1;
            if (child_pid != pid && parent_pid == pid) {
                ret = 0;
            }
            return ret;
        }
    };
    sys::process_status status = child.wait();
    expect(value(0) == value(status.exit_code()));
}

struct process_exec_params {
    sys::process_flag flags;
    std::string cmd;
    bool success;
};

std::ostream& operator<<(std::ostream& out, const process_exec_params& rhs) {
    out << "Flags: \"" << int(rhs.flags) << "\"\n";
    out << "Cmd: \"" << rhs.cmd << "\"\n";
    out << "Success: \"" << rhs.success << "\"\n";
    return out;
}

using f = sys::process_flag;
std::vector<process_exec_params> process_exec_params{
    {f::fork, "non-existent-file", false},
    {f::wait_for_exec|f::signal_parent, "non-existent-file", false},
    {f::fork, UNISTDX_TEST_EMPTY_EXE_PATH, true},
    {f::wait_for_exec|f::signal_parent, UNISTDX_TEST_EMPTY_EXE_PATH, true},
};

void test_process_exec_test_return_int() {
    for (auto param : process_exec_params) {
        sys::process_flag flags = param.flags;
        std::string cmd = param.cmd;
        bool success = param.success;
        sys::process child {
            [&] () {
                sys::argstream args;
                args.append(cmd);
                sys::this_process::execute_command(args.argv());
                return 0;
            },
            flags
        };
        sys::process_status status = child.wait();
        if (success) {
            expect(value(0) == value(status.exit_code()));
        } else {
            expect(value(0) != value(status.exit_code()));
        }
    }
}

void test_process_exec_test_return_void() {
    for (auto param : process_exec_params) {
        sys::process_flag flags = param.flags;
        std::string cmd = param.cmd;
        bool success = param.success;
        sys::process child {
            [&] () {
                sys::argstream args;
                args.append(cmd);
                sys::this_process::execute_command(args.argv());
            },
            flags
        };
        sys::process_status status = child.wait();
        if (success) {
            expect(value(0) == value(status.exit_code()));
        } else {
            expect(value(0) != value(status.exit_code()));
        }
    }
}

void test_argstream() {
    const std::string arg0 = "Hello!!!";
    const std::string arg1 = "world";
    const int arg2 = 123;
    sys::argstream args;
    expect(value(args.argc()) == value(0));
    args << arg0 << '\0' << arg1 << '\0' << arg2 << '\0';
    expect(value(args.argc()) == value(3));
    expect(value(args.argv()[0]) == value(arg0));
    expect(value(args.argv()[1]) == value(arg1));
    expect(value(args.argv()[2]) == value(std::to_string(arg2)));
    expect(value(args.argv()[args.argc()]) == value((char*)nullptr));
    args.append(arg0, arg1, arg2);
    expect(value(args.argc()) == value(6));
    expect(value(args.argv()[3]) == value(arg0));
    expect(value(args.argv()[4]) == value(arg1));
    expect(value(args.argv()[5]) == value(std::to_string(arg2)));
    expect(value(args.argv()[args.argc()]) == value((char*)nullptr));
}

void test_set_identity_exceptions() {
    using namespace sys::this_process;
    sys::uid_type other_uid = user() + 1;
    sys::gid_type other_gid = group() + 1;
    sys::uid_type old_uid = user();
    sys::gid_type old_gid = group();
    expect(throws<sys::bad_call>(call([&] () { set_identity(other_uid, other_gid); })));
    expect(value(old_uid) == value(user()));
    expect(value(old_gid) == value(group()));
    expect(throws<sys::bad_call>(call([&] () { set_identity(1000, other_gid); })));
    expect(value(old_uid) == value(user()));
    expect(value(old_gid) == value(group()));
    set_identity(user(), group());
    expect(value(old_uid) == value(user()));
    expect(value(old_gid) == value(group()));
}

#if defined(UNISTDX_TEST_HAVE_UNSHARE)
void test_unshare_hostname() {
    using namespace sys::this_process;
    auto status = sys::process{[] () {
        using f = sys::unshare_flag;
        unshare(f::users | f::hostname);
        hostname("unistdx");
        return (hostname() == "unistdx") ? 0 : 1;
    }}.wait();
    expect(value(0) == value(status.exit_code()));
}

void test_clone_users() {
    using namespace sys::this_process;
    using pf = sys::process_flag;
    sys::pipe pipe;
    pipe.in().unsetf(sys::open_flag::non_blocking);
    pipe.out().unsetf(sys::open_flag::non_blocking);
    sys::process child{[&pipe] () {
        char ch;
        pipe.in().read(&ch, 1);
        sys::log_message("clone-users", "_ _", user(), group());
        return (user() == 0 && group() == 0) ? 0 : 1;
    }, pf::signal_parent | pf::unshare_users};
    child.init_user_namespace();
    pipe.out().write("x", 1);
    auto status = child.wait();
    expect(value(0) == value(status.exit_code()));
}

std::mutex mtx;
std::condition_variable cv;
bool stopped = false;

void test_process_as_thread() {
    using pf = sys::process_flag;
    std::clog << "parent mutex    = " << &mtx << std::endl;
    std::clog << "parent cv       = " << &cv << std::endl;
    sys::process t{[] () -> int {
        std::clog << "child mutex     = " << std::addressof(mtx) << std::endl;
        std::clog << "child cv        = " << std::addressof(cv) << std::endl;
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] () { return stopped; });
        return 0;
    }, pf::signal_parent | pf::share_memory};
    {
        std::unique_lock<std::mutex> lock(mtx);
        stopped = true;
        cv.notify_one();
    }
    auto status = t.wait();
    expect(value(0) == value(status.exit_code()));
}
#endif

template <class Set>
void do_test_cpu_set() {
    expect(value("") == value(test::stream_insert(Set{})));
    expect(value("0") == value(test::stream_insert(Set{0})));
    expect(value("10") == value(test::stream_insert(Set{10})));
    expect(value("1-2") == value(test::stream_insert(Set{1,2})));
    expect(value("1-3") == value(test::stream_insert(Set{1,2,3})));
    expect(value("1,3") == value(test::stream_insert(Set{1,3})));
    expect(value("1,3-4") == value(test::stream_insert(Set{1,3,4})));
    expect(value(Set{}) == value(Set{}));
    expect(value(Set{1}) == value(Set{1}));
    expect(value(Set{1,2}) == value(Set{1,2}));
    expect(value(Set{}) == value(test::stream_extract<Set>("")));
    expect(value(Set{1}) == value(test::stream_extract<Set>("1")));
    expect(value(Set{1,2}) == value(test::stream_extract<Set>("1,2")));
    expect(value(Set{1,2,3}) == value(test::stream_extract<Set>("1,2,3")));
    expect(value(Set{1,2,3}) == value(test::stream_extract<Set>("1-3")));
    expect(value(Set{1,2,4,5}) == value(test::stream_extract<Set>("1-2,4-5")));
    expect(value(Set{1,2,4,5,10}) == value(test::stream_extract<Set>("1-2,4-5,10")));
    expect(value(Set{1,2,4,5}) == value(test::stream_extract<Set>("2-1,4-5")));
    expect(value(Set{4}) == value(Set{4}&Set{1,2,4,5,10}));
    expect(value(Set{0,1,2} & ~Set{1}) == value(Set{0,2}));
    auto all = ~Set();
    expect(value(Set{0,1,2} & ~all) == value(Set{}));
}

void test_cpu_set() {
    do_test_cpu_set<sys::static_cpu_set>();
    do_test_cpu_set<sys::dynamic_cpu_set>();
}

void test_cpu_set_identities() {
    using namespace sys::test;
    auto prng = current_test->prng();
    falsify(
        [&prng] (const Argument_array<1>&) {
            std::bernoulli_distribution dist;
            sys::static_cpu_set aa, bb;
            for (int i=0; i<aa.size(); ++i) {
                if (dist(prng)) { aa.set(i); }
            }
            for (int i=0; i<bb.size(); ++i) {
                if (dist(prng)) { bb.set(i); }
            }
            for (auto pair : {std::make_pair(&aa,&bb),
                              std::make_pair(&aa,&aa),
                              std::make_pair(&bb,&bb)}) {
                auto& a = *pair.first;
                auto& b = *pair.second;
                expect(value(~(a & b)) == value(~a | ~b));
                expect(value(a ^ b) == value((a | b) & ~(a & b)));
                expect(value(a ^ b) == value((a & ~b) | (~a & b)));
                expect(value(a & b) == value(~(~a | ~b)));
                expect(value(a) == value(~(~a)));
            }
        },
        make_parameter<int>(0,sys::static_cpu_set::max_size()));
}

void test_cpu_set_io() {
    using namespace sys::test;
    falsify(
        [] (const Argument_array<16>& params) {
            sys::static_cpu_set a;
            for (size_t i=0; i<params.size(); ++i) {
                std::bitset<64> bits(params[i]);
                for (size_t j=0; j<bits.size(); ++j) {
                    if (bits[j]) { a.set(i*bits.size() + j); }
                }
            }
            test::stream_insert_extract(a);
        },
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>(),
        make_parameter<uint64_t>());
}

void test_dynamic_cpu_set_identities() {
    using namespace sys::test;
    auto prng = current_test->prng();
    falsify(
        [&prng] (const Argument_array<1>& params) {
            std::bernoulli_distribution dist;
            sys::dynamic_cpu_set aa(params[0]), bb(params[0]);
            for (size_t i=0; i<aa.size(); ++i) {
                if (dist(prng)) { aa.set(i); }
            }
            for (size_t i=0; i<bb.size(); ++i) {
                if (dist(prng)) { bb.set(i); }
            }
            for (auto pair : {std::make_pair(&aa,&bb),
                              std::make_pair(&aa,&aa),
                              std::make_pair(&bb,&bb)}) {
                auto& a = *pair.first;
                auto& b = *pair.second;
                expect(value(~(a & b)) == value(~a | ~b));
                expect(value(a ^ b) == value((a | b) & ~(a & b)));
                expect(value(a ^ b) == value((a & ~b) | (~a & b)));
                expect(value(a & b) == value(~(~a | ~b)));
                expect(value(a) == value(~(~a)));
            }
        },
        make_parameter<int>(0, 4097));
}

void test_dynamic_cpu_set_io() {
    using namespace sys::test;
    auto prng = current_test->prng();
    falsify(
        [&prng] (const Argument_array<1>& params) {
            std::bernoulli_distribution dist;
            auto size = params[0];
            sys::dynamic_cpu_set a(size);
            expect(value(size) <= value(a.size()));
            for (size_t i=0; i<size; ++i) {
                if (dist(prng)) { a.set(i); }
            }
            test::stream_insert_extract(a);
        },
        make_parameter<int>(0, 4097));
}
