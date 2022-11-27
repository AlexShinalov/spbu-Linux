/*
UNISTDX — C++ library for Linux system calls.
© 2020, 2021 Ivan Gankevich

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

#include <iostream>
#include <regex>

#include <unistdx/base/log_message>
#include <unistdx/config>
#include <unistdx/ipc/process>
#include <unistdx/ipc/signal>
#include <unistdx/test/test_executor>

using namespace sys::test;

inline void trim_right(std::string& s) {
    while (!s.empty() && std::isspace(s.back())) { s.pop_back(); }
}

inline void trim_left(std::string& s) {
    std::string::size_type i = 0;
    auto n = s.size();
    for (; i<n && std::isspace(s[i]); ++i) {}
    if (i != n) { s = s.substr(i); }
}

inline void trim_both(std::string& s) {
    trim_right(s);
    trim_left(s);
}

class Duration: public sys::test::Test_executor::duration {
public:
    using base_duration = std::chrono::system_clock::duration;
    using base_duration::duration;
    inline Duration(base_duration rhs): base_duration(rhs) {}
};

auto string_to_duration(std::string s) -> Duration;

auto string_to_duration(std::string s) -> Duration {
    using namespace std::chrono;
    using d = Duration::base_duration;
    using days = std::chrono::duration<Duration::rep,std::ratio<60*60*24>>;
    trim_both(s);
    std::size_t i = 0, n = s.size();
    Duration::rep value = std::stoul(s, &i);
    std::string suffix;
    if (i != n) { suffix = s.substr(i); }
    if (suffix == "ns") { return duration_cast<d>(nanoseconds(value)); }
    if (suffix == "us") { return duration_cast<d>(microseconds(value)); }
    if (suffix == "ms") { return duration_cast<d>(milliseconds(value)); }
    if (suffix == "s" || suffix.empty()) { return duration_cast<d>(seconds(value)); }
    if (suffix == "m") { return duration_cast<d>(minutes(value)); }
    if (suffix == "h") { return duration_cast<d>(hours(value)); }
    if (suffix == "d") { return duration_cast<d>(days(value)); }
    std::stringstream tmp;
    tmp << "unknown duration suffix \"" << suffix << "\"";
    throw std::invalid_argument(tmp.str());
}

bool string_to_bool(std::string s) {
    trim_both(s);
    for (auto& ch : s) { ch = std::tolower(ch); }
    if (s == "1" || s == "yes" || s == "on" || s == "true") { return true; }
    if (s == "0" || s == "no" || s == "off" || s == "false") { return false; }
    throw std::invalid_argument("bad boolean");
}


std::string symbol_prefix = "test_";
std::string args_prefix = "args_";
std::regex symbol_filter{".*", std::regex::ECMAScript | std::regex::optimize};
sys::test::Test_executor tests;

template <class Function>
struct Argument {
    std::initializer_list<const char*> names;
    std::string default_value;
    const char* description;
    Function callback;
};

template <class Function>
Argument<Function> make_argument(std::initializer_list<const char*> names,
                                 std::string default_value,
                                 const char* description, Function&& callback) {
    return {names, std::move(default_value), description, callback};
}

size_t do_compute_width() { return 0; }

template <class Head, class ... Tail>
size_t do_compute_width(Head& arg, Tail& ... rest) {
    using t = std::string::traits_type;
    size_t width = 0;
    for (const char* name : arg.names) {width += t::length(name);}
    if (arg.names.size() != 0) { width += 2*(arg.names.size()-1); }
    width += (1+arg.default_value.size())*arg.names.size();
    width += 4;
    return std::max(width, do_compute_width(rest...));
}

void do_usage(std::ostream& out, size_t width) {}

template <class Head, class ... Tail>
void do_usage(std::ostream& out, size_t width, Head&& arg, Tail&& ... rest) {
    using t = std::string::traits_type;
    auto first = arg.names.begin(), last = arg.names.end();
    size_t w = 0;
    out << "  ";
    if (first != last) {
        w += t::length(*first) + 1 + arg.default_value.size();
        out << *first++ << '=' << arg.default_value;
    }
    while (first != last) {
        w += t::length(*first) + 1 + arg.default_value.size();
        w += 2;
        out << ", " << *first++ << '=' << arg.default_value;
    }
    for (size_t i=0; i<width-w; ++i) { out.put(' '); }
    out << arg.description;
    do_usage(out, width, std::forward<Tail>(rest)...);
}

template <class ... Args>
void usage(const char* name, std::ostream& out, size_t width, Args&& ... args) {
    out << name << " [-h] [--help] [--version] [key=value] ...\n";
    do_usage(out, width, std::forward<Args>(args)...);
}

inline void do_parse_arguments(const std::string& name, std::string&) {
    throw std::invalid_argument(name);
}

template <class Head, class ... Tail>
inline void do_parse_arguments(const std::string& name, std::string& value,
                               Head& arg, Tail& ... rest) {
    for (const char* arg_name : arg.names) {
        if (name == arg_name) {
            arg.callback(std::move(value));
            return;
        }
    }
    do_parse_arguments(name, value, rest...);
}

template <class ... Args>
void parse_arguments(int argc, char** argv, Args&& ... args) {
    for (int i=1; i<argc; ++i) {
        std::string arg(argv[i]);
        auto pos = arg.find('=');
        if (pos == std::string::npos) {
            if (arg == "--help" || arg == "-h") {
                auto width = do_compute_width(static_cast<Args&>(args)...);
                usage(argv[0], std::cout, width, std::forward<Args>(args)...);
                std::exit(0);
            } else if (arg == "--version") {
                std::cout << UNISTDX_VERSION "\n";
                std::exit(0);
            } else {
                throw std::invalid_argument(arg);
            }
        }
        std::string name = arg.substr(0,pos);
        std::string value = arg.substr(pos+1);
        do_parse_arguments(name, value, static_cast<Args&>(args)...);
    }
}

void arguments(int argc, char** argv) {
    sys::process::flags process_flags =
        sys::process::flags::fork | sys::process::flags::signal_parent;
    parse_arguments(
        argc, argv,
        make_argument(
            {"prefix"},
            "test_",
            "The prefix of the symbols that will be called as unit test main functions.\n",
            [&] (std::string&& value) { symbol_prefix = std::move(value); }),
        make_argument(
            {"args-prefix"},
            "args_",
            "The prefix of the symbols that will be used as unit test arguments.\n",
            [&] (std::string&& value) { args_prefix = std::move(value); }),
        make_argument(
            {"filter"},
            ".*",
            "Regular expression that is used to filter unit tests found in the executable.\n",
            [&] (std::string&& value) {
                symbol_filter = std::regex{std::move(value), symbol_filter.flags()};
            }),
        make_argument(
            {"catch-errors"},
            "yes",
            "Catch unexpected exceptions and process signals in unit tests. In rare cases disabling this option allows to see the error.\n",
            [&] (std::string&& value) {
                tests.catch_errors(string_to_bool(std::move(value)));
            }),
        make_argument(
            {"verbose"},
            "no",
            "Always print unit test output. By default only output of failed unit tests is printed.\n",
            [&] (std::string&& value) { tests.verbose(string_to_bool(std::move(value))); }),
        make_argument(
            {"redirect"},
            "yes",
            "Capture unit test output and print it only when the test failed.\n",
            [&] (std::string&& value) { tests.redirect(string_to_bool(std::move(value))); }),
        make_argument(
            {"timeout"},
            "30s",
            "Timeout for each test.\n",
            [&] (std::string&& value) { tests.timeout(string_to_duration(std::move(value))); }),
        make_argument(
            {"unshare"},
            "",
            "Comma-separated list of namespaces to unshare before running unit tests. Possible values: network, users.\n",
            [&] (std::string&& value) {
                using f = sys::process::flags;
                value += ',';
                std::string subvalue;
                for (auto ch : value) {
                    if (ch == ',') {
                        if (subvalue == "network") {
                            process_flags |= f::unshare_network;
                        } else if (subvalue == "users") {
                            process_flags |= f::unshare_users;
                        } else if (subvalue.empty()) {
                            process_flags = f::fork | f::signal_parent;
                        } else {
                            throw std::invalid_argument("unshare");
                        }
                        subvalue.clear();
                    } else {
                        subvalue += ch;
                    }
                }
            })
    );
    tests.process_flags(process_flags);
}

void parent_signal_handlers() {
    using namespace sys::this_process;
    auto on_terminate = sys::signal_action([](int sig) {
        try {
            tests.send(sys::signal(sig));
        } catch (const std::exception& err) {
            const char* msg = "error in parent signal handler";
            ::write(2, msg, std::string::traits_type::length(msg));
            ::write(2, err.what(), std::string::traits_type::length(err.what()));
        }
        tests.wait();
        std::_Exit(sig);
    });
    using s = sys::signal;
    ignore_signal(s::broken_pipe);
    bind_signal(s::keyboard_interrupt, on_terminate);
    bind_signal(s::terminate, on_terminate);
    bind_signal(s::quit, on_terminate);
    bind_signal(s::hang_up, on_terminate);
}

int main(int argc, char* argv[]) {
    arguments(argc, argv);
    sys::string buf{4096};
    dl::for_each_shared_object([&] (const elf::shared_object& obj, size_t nobjects) {
        for (const auto& prg : obj) {
            if (prg.type() != elf::segment_types::dynamic) { continue; }
            auto dynamic_section = reinterpret_cast<elf::dynamic_section*>(
                obj.base_address() + prg.virtual_address());
            using tags = elf::dynamic_section_tags;
            elf::elf_word* hash{};
            elf::elf_word num_symbols{};
            char* strings{};
            elf::symbol* symbols{};
            for (; *dynamic_section; ++dynamic_section) {
                //std::clog << "tag: " << int(dynamic_section->tag()) << std::endl;
                switch (dynamic_section->tag()) {
                    case tags::hash:
                        hash = dynamic_section->address<elf::elf_word>();
                        num_symbols = hash[1];
                        break;
                    case tags::string_table:
                        strings = dynamic_section->address<char>();
                        break;
                    case tags::symbol_table:
                        symbols = dynamic_section->address<elf::symbol>();
                        break;
                    default:
                        break;
                }
            }
            if (symbols && strings) {
                // functions
                for (elf::elf_word i=0; i<num_symbols; ++i) {
                    const auto& sym = symbols[i];
                    auto name = &strings[sym.name()];
                    if (*name == 0) { continue; }
                    std::string demangled_name = sys::demangle(name, buf);
                    if (demangled_name.compare(0, symbol_prefix.size(), symbol_prefix) == 0) {
                        sys::test::Symbol s;
                        s.original_name(std::string(name));
                        s.demangled_name(std::move(demangled_name));
                        s.address(reinterpret_cast<void*>(sym.address()));
                        s.type(sym.type());
                        if (std::regex_search(s.short_name(), symbol_filter)) {
                            tests.emplace(std::move(s));
                        }
                    }
                }
                // arguments
                for (elf::elf_word i=0; i<num_symbols; ++i) {
                    const auto& sym = symbols[i];
                    auto name = &strings[sym.name()];
                    if (*name == 0) { continue; }
                    std::string demangled_name = sys::demangle(name, buf);
                    if (demangled_name.compare(0, args_prefix.size(), args_prefix) == 0) {
                        sys::test::Symbol s;
                        s.original_name(std::string(name));
                        s.demangled_name(std::move(demangled_name));
                        s.address(reinterpret_cast<void*>(sym.address()));
                        s.type(sym.type());
                        std::string test_name;
                        test_name.reserve(symbol_prefix.size() + args_prefix.size());
                        test_name += symbol_prefix;
                        test_name += s.short_name().substr(args_prefix.size());
                        tests.test_arguments(test_name, std::move(s));
                    }
                }
            }
        }
        return 1;
    });
    parent_signal_handlers();
    return tests.run();
}
