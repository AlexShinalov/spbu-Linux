/*
UNISTDX — C++ library for Linux system calls.
© 2021 Ivan Gankevich

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

#include <unistdx/fs/idirectory>
#include <unistdx/fs/path_view>
#include <unistdx/io/event_file_descriptor>
#include <unistdx/ipc/process>
#include <unistdx/system/error>

int main(int argc, char* argv[]) {
    using f = sys::process::flags;
    sys::event_file_descriptor notifier(
        0, sys::event_file_descriptor::flag::close_on_exec);
    sys::process child{
        [&notifier] () {
            //std::set_terminate(sys::backtrace_on_terminate);
            notifier.read();
            notifier.close();
            std::clog << "Hello world!" << std::endl;
            sys::path_view("/tmp").set_root();
            sys::idirectory root("/");
            for (const auto& entry : root) {
                std::clog << entry << '\n';
            }
            std::clog << std::flush;
            return 0;
        },
        f::signal_parent | f::unshare_mount_points | f::unshare_users
    };
    child.init_user_namespace();
    notifier.write(1);
    child.wait();
    return 0;
}
