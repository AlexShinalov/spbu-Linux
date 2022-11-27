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

#include <cstring>
#include <limits>
#include <ostream>

#include <unistdx/system/security>

const char* sys::to_string(capabilities rhs) noexcept {
    using c = capabilities;
    switch (rhs) {
        case c::chown: return "cap_chown";
        case c::dac_override: return "cap_dac_override";
        case c::dac_read_search: return "cap_dac_read_search";
        case c::fowner: return "cap_fowner";
        case c::fsetid: return "cap_fsetid";
        case c::kill: return "cap_kill";
        case c::setgid: return "cap_setgid";
        case c::setuid: return "cap_setuid";
        case c::setpcap: return "cap_setpcap";
        case c::linux_immutable: return "cap_linux_immutable";
        case c::net_bind_service: return "cap_net_bind_service";
        case c::net_broadcast: return "cap_net_broadcast";
        case c::net_admin: return "cap_net_admin";
        case c::net_raw: return "cap_net_raw";
        case c::ipc_lock: return "cap_ipc_lock";
        case c::ipc_owner: return "cap_ipc_owner";
        case c::sys_module: return "cap_sys_module";
        case c::sys_rawio: return "cap_sys_rawio";
        case c::sys_chroot: return "cap_sys_chroot";
        case c::sys_ptrace: return "cap_sys_ptrace";
        case c::sys_pacct: return "cap_sys_pacct";
        case c::sys_admin: return "cap_sys_admin";
        case c::sys_boot: return "cap_sys_boot";
        case c::sys_nice: return "cap_sys_nice";
        case c::sys_resource: return "cap_sys_resource";
        case c::sys_time: return "cap_sys_time";
        case c::sys_tty_config: return "cap_sys_tty_config";
        case c::mknod: return "cap_mknod";
        case c::lease: return "cap_lease";
        case c::audit_write: return "cap_audit_write";
        case c::audit_control: return "cap_audit_control";
        case c::setfcap: return "cap_setfcap";
        case c::mac_override: return "cap_mac_override";
        case c::mac_admin: return "cap_mac_admin";
        case c::syslog: return "cap_syslog";
        case c::wake_alarm: return "cap_wake_alarm";
        case c::block_suspend: return "cap_block_suspend";
        case c::audit_read: return "cap_audit_read";
        default: return nullptr;
    }
}

std::ostream& sys::operator<<(std::ostream& out, capabilities rhs) {
    auto s = to_string(rhs);
    return out << (s ? s : "unknown");
}

sys::capabilities sys::string_to_capability(const char* s) noexcept {
    using std::strcmp;
    using c = capabilities;
    if (strcmp(s, "cap_chown") == 0) { return c::chown; }
    if (strcmp(s, "cap_dac_override") == 0) { return c::dac_override; }
    if (strcmp(s, "cap_dac_read_search") == 0) { return c::dac_read_search; }
    if (strcmp(s, "cap_fowner") == 0) { return c::fowner; }
    if (strcmp(s, "cap_fsetid") == 0) { return c::fsetid; }
    if (strcmp(s, "cap_kill") == 0) { return c::kill; }
    if (strcmp(s, "cap_setgid") == 0) { return c::setgid; }
    if (strcmp(s, "cap_setuid") == 0) { return c::setuid; }
    if (strcmp(s, "cap_setpcap") == 0) { return c::setpcap; }
    if (strcmp(s, "cap_linux_immutable") == 0) { return c::linux_immutable; }
    if (strcmp(s, "cap_net_bind_service") == 0) { return c::net_bind_service; }
    if (strcmp(s, "cap_net_broadcast") == 0) { return c::net_broadcast; }
    if (strcmp(s, "cap_net_admin") == 0) { return c::net_admin; }
    if (strcmp(s, "cap_net_raw") == 0) { return c::net_raw; }
    if (strcmp(s, "cap_ipc_lock") == 0) { return c::ipc_lock; }
    if (strcmp(s, "cap_ipc_owner") == 0) { return c::ipc_owner; }
    if (strcmp(s, "cap_sys_module") == 0) { return c::sys_module; }
    if (strcmp(s, "cap_sys_rawio") == 0) { return c::sys_rawio; }
    if (strcmp(s, "cap_sys_chroot") == 0) { return c::sys_chroot; }
    if (strcmp(s, "cap_sys_ptrace") == 0) { return c::sys_ptrace; }
    if (strcmp(s, "cap_sys_pacct") == 0) { return c::sys_pacct; }
    if (strcmp(s, "cap_sys_admin") == 0) { return c::sys_admin; }
    if (strcmp(s, "cap_sys_boot") == 0) { return c::sys_boot; }
    if (strcmp(s, "cap_sys_nice") == 0) { return c::sys_nice; }
    if (strcmp(s, "cap_sys_resource") == 0) { return c::sys_resource; }
    if (strcmp(s, "cap_sys_time") == 0) { return c::sys_time; }
    if (strcmp(s, "cap_sys_tty_config") == 0) { return c::sys_tty_config; }
    if (strcmp(s, "cap_mknod") == 0) { return c::mknod; }
    if (strcmp(s, "cap_lease") == 0) { return c::lease; }
    if (strcmp(s, "cap_audit_write") == 0) { return c::audit_write; }
    if (strcmp(s, "cap_audit_control") == 0) { return c::audit_control; }
    if (strcmp(s, "cap_setfcap") == 0) { return c::setfcap; }
    if (strcmp(s, "cap_mac_override") == 0) { return c::mac_override; }
    if (strcmp(s, "cap_mac_admin") == 0) { return c::mac_admin; }
    if (strcmp(s, "cap_syslog") == 0) { return c::syslog; }
    if (strcmp(s, "cap_wake_alarm") == 0) { return c::wake_alarm; }
    if (strcmp(s, "cap_block_suspend") == 0) { return c::block_suspend; }
    if (strcmp(s, "cap_audit_read") == 0) { return c::audit_read; }
    using t = std::underlying_type<capabilities>::type;
    return capabilities(std::numeric_limits<t>::max());
}

std::istream& sys::operator>>(std::istream& in, capabilities& rhs) {
    std::string s;
    in >> s;
    rhs = string_to_capability(s.data());
    return in;
}
