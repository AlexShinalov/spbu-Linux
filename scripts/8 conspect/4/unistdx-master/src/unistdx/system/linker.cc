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

#include <ostream>

#include <unistdx/system/linker>

dl::error_category dl::dl_category;

const char* dl::error_category::name() const noexcept {
    return "dl";
}

std::string dl::error_category::message(int) const noexcept {
    return std::string("libdl error");
}

const char* elf::to_string(symbol_types rhs) noexcept {
    switch (rhs) {
        #if defined(STT_NOTYPE)
        case symbol_types::none: return "none";
        #endif
        #if defined(STT_OBJECT)
        case symbol_types::data: return "data";
        #endif
        #if defined(STT_FUNC)
        case symbol_types::code: return "code";
        #endif
        #if defined(STT_SECTION)
        case symbol_types::section: return "section";
        #endif
        #if defined(STT_FILE)
        case symbol_types::file: return "file";
        #endif
        #if defined(STT_COMMON)
        case symbol_types::common: return "common";
        #endif
        #if defined(STT_TLS)
        case symbol_types::tls: return "tls";
        #endif
        #if defined(STT_NUM)
        case symbol_types::num: return "num";
        #endif
        #if defined(STT_LOOS)
        case symbol_types::loos: return "loos";
        #endif
        #if defined(STT_HIOS)
        case symbol_types::hios: return "hios";
        #endif
        #if defined(STT_LOPROC)
        case symbol_types::loproc: return "loproc";
        #endif
        #if defined(STT_HIPROC)
        case symbol_types::hiproc: return "hiproc";
        #endif
        default: return nullptr;
    }
}

std::ostream& elf::operator<<(std::ostream& out, symbol_types rhs) {
    auto s = to_string(rhs);
    return out << (s ? s : "unknown");
}
