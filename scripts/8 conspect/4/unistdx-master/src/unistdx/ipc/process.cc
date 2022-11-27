/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2020, 2021 Ivan Gankevich

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

#include <limits.h>

#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>
#include <unistdx/system/resource>

sys::fildes sys::this_process::get_namespace(const char* suffix) {
    char path[100];
    std::sprintf(path, "/proc/self/ns/%s", suffix);
    return sys::fildes(path);
}

sys::fildes sys::process_view::get_namespace(const char* suffix) {
    char path[100];
    std::sprintf(path, "/proc/%d/ns/%s", id(), suffix);
    return sys::fildes(path);
}

std::string sys::this_process::hostname() {
    std::string name;
    name.resize(HOST_NAME_MAX);
    check(::gethostname(&name[0], HOST_NAME_MAX));
    name[HOST_NAME_MAX-1] = 0;
    name.resize(std::string::traits_type::length(name.data()));
    return name;
}

void sys::process_view::init_user_namespace() {
    char buf[100];
    fildes out;
    int n;
    std::sprintf(buf, "/proc/%d/uid_map", id());
    out.open(buf, open_flag::write_only);
    n = std::sprintf(buf, "0 %d 1", sys::this_process::user());
    out.write(buf, n);
    std::sprintf(buf, "/proc/%d/setgroups", id());
    out.open(buf, open_flag::write_only);
    out.write("deny", 4);
    std::sprintf(buf, "/proc/%d/gid_map", id());
    out.open(buf, open_flag::write_only);
    n = std::sprintf(buf, "0 %d 1", sys::this_process::group());
    out.write(buf, n);
    out.close();
}

sys::dynamic_cpu_set sys::process_view::cpus() const {
    auto size = dynamic_cpu_set::min_size();
    dynamic_cpu_set mask(size);
    int ret;
    do {
        ret = ::sched_getaffinity(this->_pid, mask.size_in_bytes(), mask.get());
        if (ret == EINVAL) {
            size *= 2;
            mask = dynamic_cpu_set(size);
        }
    } while (ret == EINVAL);
    check(ret);
    return mask;
}
