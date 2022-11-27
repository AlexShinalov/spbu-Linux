/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2019, 2020, 2021 Ivan Gankevich

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
#include <sstream>

#include <unistdx/system/security>
#include <unistdx/test/arguments>
#include <unistdx/test/language>
#include <unistdx/test/operator>

using namespace sys::test::lang;

void show_capabilities() {
    using namespace sys::this_process;
    std::clog << "Bounding set\n";
    for (sys::u32 i=0; i<=37; ++i) {
        std::clog << i << ": " << bounding_set_contains(sys::capabilities(i)) << '\n';
    }
    std::clog << "Ambient set\n";
    for (sys::u32 i=0; i<=37; ++i) {
        std::clog << i << ": " << ambient_set_contains(sys::capabilities(i)) << '\n';
    }
    std::clog << "Security bits: " << sys::u32(security_bits()) << '\n';
    std::clog << "Keep capabilities: " << keep_capabilities() << '\n';
}

void test_capabilities() {
    using namespace sys::this_process;
    //using s = sys::security_bits;
    show_capabilities();
    //security_bits(s::keep);
    //security_bits(s::no_root | s::no_root_locked);
    //ambient_set_add(sys::capabilities::chown);
    show_capabilities();
}

arguments<sys::capabilities> args_capabilities_io = {
    sys::capabilities::chown,
    sys::capabilities::dac_override,
    sys::capabilities::dac_read_search,
    sys::capabilities::fowner,
    sys::capabilities::fsetid,
    sys::capabilities::kill,
    sys::capabilities::setgid,
    sys::capabilities::setuid,
    sys::capabilities::setpcap,
    sys::capabilities::linux_immutable,
    sys::capabilities::net_bind_service,
    sys::capabilities::net_broadcast,
    sys::capabilities::net_admin,
    sys::capabilities::net_raw,
    sys::capabilities::ipc_lock,
    sys::capabilities::ipc_owner,
    sys::capabilities::sys_module,
    sys::capabilities::sys_rawio,
    sys::capabilities::sys_chroot,
    sys::capabilities::sys_ptrace,
    sys::capabilities::sys_pacct,
    sys::capabilities::sys_admin,
    sys::capabilities::sys_boot,
    sys::capabilities::sys_nice,
    sys::capabilities::sys_resource,
    sys::capabilities::sys_time,
    sys::capabilities::sys_tty_config,
    sys::capabilities::mknod,
    sys::capabilities::lease,
    sys::capabilities::audit_write,
    sys::capabilities::audit_control,
    sys::capabilities::setfcap,
    sys::capabilities::mac_override,
    sys::capabilities::mac_admin,
    sys::capabilities::syslog,
    sys::capabilities::wake_alarm,
    sys::capabilities::block_suspend,
    sys::capabilities::audit_read,
    sys::capabilities(-1),
};

bool test_capabilities_io(sys::capabilities* cap) {
    return test::io_operators(*cap);
}
