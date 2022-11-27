/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2019, 2020 Ivan Gankevich

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

#include <unistdx/io/two_way_pipe>

#include <unistdx/base/make_object>

void
sys::two_way_pipe::validate() {
    if (this->is_owner()) {
        this->parent_in().validate();
        this->parent_out().validate();
    } else {
        this->child_in().validate();
        this->child_out().validate();
    }
}

void
sys::two_way_pipe::open() {
    this->_owner = this_process::id();
    this->_pipe1.open();
    this->_pipe2.open();
}

void
sys::two_way_pipe::close() {
    this->_pipe1.close();
    this->_pipe2.close();
}

void
sys::two_way_pipe::close_in_child() {
    this->_pipe1.in().close();
    this->_pipe2.out().close();
}

void
sys::two_way_pipe::close_in_parent() {
    this->_pipe1.out().close();
    this->_pipe2.in().close();
}

void
sys::two_way_pipe::close_unused() {
    this->is_owner() ? this->close_in_parent() : this->close_in_child();
}

std::ostream&
sys::operator<<(std::ostream& out, const two_way_pipe& rhs) {
    return out << make_object("pipe1", rhs._pipe1, "pipe2", rhs._pipe2);
}
