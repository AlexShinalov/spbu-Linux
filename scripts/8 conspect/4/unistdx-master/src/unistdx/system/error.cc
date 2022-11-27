/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020, 2021 Ivan Gankevich

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

#if defined(UNISTDX_HAVE_PRCTL)
#include <sys/prctl.h>
#endif

#include <iostream>
#include <ostream>
#include <sstream>

#include <unistdx/ipc/process>
#include <unistdx/system/error>
#include <unistdx/system/resource>

#include <unistdx/config>
#if defined(UNISTDX_HAVE_BACKTRACE)
#include <execinfo.h>
#endif
#if defined(UNISTDX_HAVE_CXXABI_H)
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <string>
#endif

#if defined(UNISTDX_WITH_LIBDW)
#include <elfutils/libdwfl.h>
#include <unistd.h>
#endif

#include <unistdx/system/linker>

namespace {

    #if !defined(UNISTDX_HAVE_BACKTRACE)
    inline void write_fd(int fd, const char* msg, size_t n) {
        ssize_t nwritten;
        while (n > 0 && (nwritten = ::write(fd, msg, n)) != -1) {
            msg += nwritten;
            n -= nwritten;
        }
    }

    inline void write_fd(int fd, const char* msg) {
        write_fd(fd, msg, std::strlen(msg));
    }
    #endif

}

void sys::backtrace(int fd) noexcept {
    #if defined(UNISTDX_HAVE_BACKTRACE)
    const size_t size = 4096 / sizeof(void*);
    void* buffer[size];
    const int nptrs = ::backtrace(buffer, size);
    ::backtrace_symbols_fd(buffer, nptrs, fd);
    #else
    write_fd(fd, "Backtrace: <none>\n");
    #endif
}

std::vector<sys::backtrace_symbol>
sys::stack_trace::symbols() const noexcept {
    auto addresses = data();
    auto nptrs = size();
    #if defined(UNISTDX_HAVE_BACKTRACE)
    try {
        std::vector<backtrace_symbol> symb;
        symb.reserve(nptrs);
        string buf(4096);
        #if defined(UNISTDX_WITH_LIBDW)
        using dw_context = ::Dwfl*;
        using dw_address = ::Dwarf_Addr;
        ::Dwfl_Callbacks callbacks{};
        callbacks.find_elf = ::dwfl_linux_proc_find_elf;
        callbacks.find_debuginfo = ::dwfl_standard_find_debuginfo;
        #else
        using dw_context = void*;
        using dw_address = void*;
        using int_function = int (*)(...);
        using ptr_function = void* (*)(...);
        using void_function = void (*)(...);
        dl::object libdw{"libdw.so", dl::object::modes::lazy};
        if (!libdw) { return {}; }
        struct { void* a[4]; } callbacks {};
        callbacks.a[0] = libdw.symbol_address("dwfl_linux_proc_find_elf");
        callbacks.a[1] = libdw.symbol_address("dwfl_standard_find_debuginfo");
        if (!callbacks.a[0] || !callbacks.a[1]) { return {}; }
        auto dwfl_begin = libdw.function<ptr_function>("dwfl_begin");
        auto dwfl_end = libdw.function<void_function>("dwfl_end");
        auto dwfl_linux_proc_report =
            libdw.function<int_function>("dwfl_linux_proc_report");
        auto dwfl_report_end = libdw.function<int_function>("dwfl_report_end");
        auto dwfl_addrmodule = libdw.function<ptr_function>("dwfl_addrmodule");
        auto dwfl_module_info = libdw.function<ptr_function>("dwfl_module_info");
        auto dwfl_getsrc = libdw.function<ptr_function>("dwfl_getsrc");
        auto dwfl_lineinfo = libdw.function<ptr_function>("dwfl_lineinfo");
        if (!dwfl_begin || !dwfl_end || !dwfl_linux_proc_report || !dwfl_report_end ||
            !dwfl_addrmodule || !dwfl_module_info || !dwfl_getsrc || !dwfl_lineinfo) {
            return {};
        }
        #endif
        dw_context context = dwfl_begin(&callbacks);
        if (!context) { return {}; }
        if (dwfl_linux_proc_report(context, ::getpid()) == -1) { return {}; }
        if (dwfl_report_end(context, nullptr, nullptr) == -1) { return {}; }
        #if defined(UNISTDX_HAVE_DLADDR)
        for (int i=0; i<nptrs; ++i) {
            dl::symbol sym(addresses[i]);
            if (!sym) { continue; }
            auto demangled_name = demangle(sym.name(), buf);
            auto module = dwfl_addrmodule(context, dw_address(addresses[i]));
            backtrace_symbol s;
            s.address = uintptr_t(addresses[i]);
            s.name = demangled_name;
            if (module) {
                if (auto name = dwfl_module_info(module, 0,0,0,0,0,0,0)) {
                    s.object = reinterpret_cast<const char*>(name);
                }
            }
            if (auto src = dwfl_getsrc(context, dw_address(addresses[i]))) {
                int line = 0;
                if (auto filename = dwfl_lineinfo(src, 0, &line, 0,0,0)) {
                    s.filename = reinterpret_cast<const char*>(filename);
                    s.line = line;
                }
            }
            symb.emplace_back(std::move(s));
        }
        #else
        symb.emplace_back("", "-", 0, 0);
        if (char** symbols = ::backtrace_symbols(addresses, nptrs)) {
            for (int i=0; i<nptrs; ++i) {
                const char* name = symbols[i];
                if (!name) {
                    name = "<null>";
                }
                try {
                    // parse trace string
                    std::string line(name);
                    std::string::size_type pos0, pos1;
                    pos0 = line.find_first_of('(');
                    pos1 = line.find_last_of(')');
                    pos1 = line.find_last_of('+', pos1);
                    if (pos0 == std::string::npos || pos1 == std::string::npos) {
                        throw 1;
                    }
                    std::string func = line.substr(pos0+1, pos1-pos0-1);
                    auto demangled_name = demangle(func.data(), buf);
                    symb.emplace_back(line.substr(0, pos0), demangled_name, 0, uintptr_t(addresses[i]));
                } catch (...) {
                    symb.emplace_back("", name, 0, uintptr_t(addresses[i]));
                }
            }
            std::free(symbols);
        }
        #endif
        return symb;
    } catch (...) {
        return {};
    }
    #else
    return {};
    #endif
}

void sys::print(std::ostream& out, const char* message, const stack_trace& trace) noexcept {
    char process_name[16] {};
    #if defined(UNISTDX_HAVE_PRCTL)
    ::prctl(PR_GET_NAME, process_name);
    #endif
    try {
        out << "Exception in process \"";
        #if defined(UNISTDX_HAVE_PRCTL)
        out << process_name;
        #else
        out << std::this_process::id();
        #endif
        out << "\": " << message;
        out << '\n';
        out << trace;
    } catch (...) {
        // no message
    }
}

void sys::error::init() const noexcept {
    try {
        std::stringstream tmp;
        print(tmp, this->_message.data(), this->_backtrace);
        this->_message = tmp.str();
    } catch (...) {
        // no message
    }
    this->_init = true;
}

const char* sys::error::what() const noexcept {
    if (!this->_init) { init(); }
    return this->_message.data();
}

std::ostream& sys::operator<<(std::ostream& out, const backtrace_symbol& rhs) {
    if (!rhs.name.empty()) {
        out << rhs.name << ' ';
    }
    out << '(';
    if (rhs.filename.empty()) {
        out << rhs.object << ":0x" << std::hex << rhs.address << std::dec;
    } else {
        out << rhs.filename;
        if (rhs.line != 0) {
            out << ':' << rhs.line;
        }
    }
    out << ')';
    return out;
}

const char* sys::demangle(const char* symbol, string& buf) {
    #if defined(UNISTDX_HAVE_CXXABI_H)
    try {
        size_t size = buf.capacity();
        int status = 0;
        auto ptr = abi::__cxa_demangle(symbol, buf.data(), &size, &status);
        if (status == 0) {
            buf.data(ptr);
            buf.capacity(size);
            return buf.data();
        }
        return symbol;
    } catch (...) {
        return symbol;
    }
    #else
    return symbol;
    #endif
}

void sys::backtrace_on_signal(int sig) noexcept {
    char name[16] {'\0'};
    #if defined(UNISTDX_HAVE_PRCTL)
    ::prctl(PR_GET_NAME, name);
    #endif
    auto name_size = std::char_traits<char>::length(name);
    ::write(STDERR_FILENO, "Caught ", 7);
    auto signal_name = to_string(sys::signal(sig));
    auto signal_name_size = std::char_traits<char>::length(signal_name);
    ::write(STDERR_FILENO, signal_name, signal_name_size);
    ::write(STDERR_FILENO, " in process \"", 13);
    ::write(STDERR_FILENO, name, name_size);
    ::write(STDERR_FILENO, "\"\n", 2);
    ::sys::backtrace(STDERR_FILENO);
    std::exit(sig);
}

void sys::backtrace_on_terminate() {
    if (auto ptr = std::current_exception()) {
        try {
            std::rethrow_exception(ptr);
        } catch (const error& err) {
            std::cerr << err.what();
        } catch (const std::exception& err) {
            std::cerr << error(err.what()).what();
        } catch (...) {
            std::cerr << error("Unknown uncaught exception").what();
        }
    } else {
        std::cerr << error("No exception").what();
    }
    std::cerr << std::flush;
    std::_Exit(1);
}

void sys::dump_core() noexcept {
    try {
        this_process::limit(resources::core_file_size,
                            {resource_limit::infinity,resource_limit::infinity});
        this_process::default_action(signal::abort);
        this_process::send(signal::abort);
    } catch (const std::exception& err) {
        using t = std::string::traits_type;
        auto msg = "failed to dump core: ";
        ::write(STDERR_FILENO, msg, t::length(msg));
        msg = err.what();
        ::write(STDERR_FILENO, msg, t::length(msg));
        std::_Exit(1);
    } catch (...) {
        using t = std::string::traits_type;
        auto msg = "failed to dump core: unknown error";
        ::write(STDERR_FILENO, msg, t::length(msg));
        std::_Exit(1);
    }
}

std::ostream& sys::operator<<(std::ostream& out, const stack_trace& rhs) {
    const auto& symbols = rhs.symbols();
    int i = 0;
    for (const auto& s : symbols) { out << "    #" << i++ << ' ' << s << '\n'; }
    out << "Backtrace:";
    for (auto address : rhs) { out << ' ' << address; }
    out << '\n';
    return out;
}
