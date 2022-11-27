/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

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

#include <unistdx/ipc/signal>

#include <ostream>

const char* sys::to_string(signal rhs) noexcept {
    switch (rhs) {
        case signal::hang_up: return "hang_up";
        case signal::keyboard_interrupt: return "keyboard_interrupt";
        case signal::quit: return "quit";
        case signal::illegal_instruction: return "illegal_instruction";
        case signal::breakpoint: return "breakpoint";
        case signal::abort: return "abort";
        case signal::floating_point_exception: return "floating_point_exception";
        case signal::kill: return "kill";
        case signal::bad_memory_access: return "bad_memory_access";
        case signal::segmentation_fault: return "segmentation_fault";
        case signal::bad_argument: return "bad_argument";
        case signal::broken_pipe: return "broken_pipe";
        case signal::alarm: return "alarm";
        case signal::terminate: return "terminate";
        case signal::urgent: return "urgent";
        #if defined(UNISTDX_HAVE_SIGSTKFLT)
        case signal::coprocessor_stack_fault: return "coprocessor_stack_fault";
        #endif
        case signal::stop: return "stop";
        case signal::terminal_stop: return "terminal_stop";
        case signal::resume: return "resume";
        case signal::child: return "child";
        case signal::terminal_input: return "terminal_input";
        case signal::terminal_output: return "terminal_output";
        #if defined(UNISTDX_HAVE_SIGPOLL)
        case signal::poll: return "poll";
        #endif
        case signal::cpu_time_limit_exceeded: return "cpu_time_limit_exceeded";
        case signal::file_size_limit_exceeded: return "file_size_limit_exceeded";
        case signal::virtual_alarm: return "virtual_alarm";
        case signal::profile: return "profile";
        #if defined(UNISTDX_HAVE_SIGWINCH)
        case signal::terminal_window_resize: return "terminal_window_resize";
        #endif
        case signal::user_defined_1: return "user_defined_1";
        #if defined(UNISTDX_HAVE_SIGPWR)
        case signal::power_failure: return "power_failure";
        #endif
        case signal::user_defined_2: return "user_defined_2";
        default: return "unknown";
    }
}

std::ostream&
sys::operator<<(std::ostream& out, const signal rhs) {
    return out << to_string(rhs) << '(' << signal_type(rhs) << ')';
}
