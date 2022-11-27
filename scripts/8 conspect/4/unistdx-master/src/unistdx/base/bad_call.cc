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

#include <unistdx/base/bad_call>

#if defined(UNISTDX_ENABLE_BAD_CALL_BACKTRACE)
const char* sys::bad_call::what() const noexcept {
    return this->_error.what();
}
#else
const char* sys::bad_call::what() const noexcept {
    return to_string(this->_code);
}
#endif

const char* sys::to_string(errors e) noexcept {
    switch (e) {
        case errors::opeartion_not_permitted: return "Operation not permitted";
        case errors::no_such_file_or_directory: return "No such file or directory";
        case errors::no_such_process: return "No such process";
        case errors::interrupted: return "Interrupted system call";
        case errors::io: return "I/O error";
        case errors::no_such_device_or_address: return "No such device or address";
        case errors::argument_list_too_long: return "Argument list too long";
        case errors::executable_format_error: return "Exec format error";
        case errors::bad_file_descriptor: return "Bad file number";
        case errors::no_child_processes: return "No child processes";
        case errors::try_again: return "Try again";
        case errors::out_of_memmory: return "Out of memory";
        case errors::permission_denied: return "Permission denied";
        case errors::bad_address: return "Bad address";
        case errors::block_device_required: return "Block device required";
        case errors::device_or_resources_busy: return "Device or resource busy";
        case errors::file_exists: return "File exists";
        case errors::cross_device_linke: return "Cross-device link";
        case errors::no_such_devices: return "No such device";
        case errors::not_a_directory: return "Not a directory";
        case errors::is_a_directory: return "Is a directory";
        case errors::invalid_argument: return "Invalid argument";
        case errors::file_table_overflow: return "File table overflow";
        case errors::too_many_open_files: return "Too many open files";
        case errors::not_a_typewriter: return "Not a typewriter";
        case errors::text_file_busy: return "Text file busy";
        case errors::file_too_large: return "File too large";
        case errors::no_space_left_on_device: return "No space left on device";
        case errors::illegal_seek: return "Illegal seek";
        case errors::read_only_file_system: return "Read-only file system";
        case errors::too_many_links: return "Too many links";
        case errors::broken_pipe: return "Broken pipe";
        case errors::domain: return "Math argument out of domain of func";
        case errors::range: return "Math result not representable";
        case errors::deadlock: return "Resource deadlock would occur";
        case errors::file_name_too_long: return "File name too long";
        case errors::no_lock_available: return "No record locks available";
        case errors::invalid_system_call: return "Invalid system call number";
        case errors::directory_not_emty: return "Directory not empty";
        case errors::too_many_symbolic_links: return "Too many symbolic links encountered";
        case errors::no_message: return "No message of desired type";
        case errors::identifier_removed: return "Identifier removed";
        case errors::channel_number_out_of_range: return "Channel number out of range";
        case errors::level_2_not_synchronized: return "Level 2 not synchronized";
        case errors::level_3_halted: return "Level 3 halted";
        case errors::level_3_reset: return "Level 3 reset";
        case errors::link_number_out_of_range: return "Link number out of range";
        case errors::protocol_driver_not_attached: return "Protocol driver not attached";
        case errors::no_csi_structure_available: return "No CSI structure available";
        case errors::level_2_halted: return "Level 2 halted";
        case errors::invalid_exchange: return "Invalid exchange";
        case errors::invalid_request_descriptor: return "Invalid request descriptor";
        case errors::exchange_full: return "Exchange full";
        case errors::no_anode: return "No anode";
        case errors::invalid_request_code: return "Invalid request code";
        case errors::invalid_slot: return "Invalid slot";
        case errors::bad_font_file_format: return "Bad font file format";
        case errors::device_not_a_stream: return "Device not a stream";
        case errors::no_data: return "No data available";
        case errors::timer_expired: return "Timer expired";
        case errors::out_of_stream_resources: return "Out of streams resources";
        case errors::machine_is_not_on_the_network: return "Machine is not on the network";
        case errors::package_not_installed: return "Package not installed";
        case errors::object_is_remote: return "Object is remote";
        case errors::no_link: return "Link has been severed";
        case errors::advertise_error: return "Advertise error";
        case errors::srmount: return "Srmount error";
        case errors::communication: return "Communication error on send";
        case errors::protocol: return "Protocol error";
        case errors::multihop: return "Multihop attempted";
        case errors::rfs: return "RFS specific error";
        case errors::not_a_data_message: return "Not a data message";
        case errors::overflow: return "Value too large for defined data type";
        case errors::name_not_unique_on_network: return "Name not unique on network";
        case errors::file_descriptor_in_bad_state: return "File descriptor in bad state";
        case errors::remote_address_changed: return "Remote address changed";
        case errors::can_not_access_shared_library: return "Can not access a needed shared library";
        case errors::accessing_a_corrupted_shared_library: return "Accessing a corrupted shared library";
        case errors::lib_section_corrupted: return ".lib section in a.out corrupted";
        case errors::too_many_shared_libararies: return "Attempting to link in too many shared libraries";
        case errors::can_not_execute_shared_library_directly: return "Cannot exec a shared library directly";
        case errors::illegal_byte_sequence: return "Illegal byte sequence";
        case errors::interrupted_system_call_should_be_restarted: return "Interrupted system call should be restarted";
        case errors::stream_pipe: return "Streams pipe error";
        case errors::too_many_users: return "Too many users";
        case errors::not_a_socket: return "Socket operation on non-socket";
        case errors::destination_address_required: return "Destination address required";
        case errors::message_too_long: return "Message too long";
        case errors::protocol_wrong_type: return "Protocol wrong type for socket";
        case errors::protocol_not_available: return "Protocol not available";
        case errors::protocol_not_supported: return "Protocol not supported";
        case errors::socket_type_not_supported: return "Socket type not supported";
        case errors::operation_not_supported: return "Operation not supported on transport endpoint";
        case errors::protocol_family_not_supported: return "Protocol family not supported";
        case errors::address_family_not_supported: return "Address family not supported by protocol";
        case errors::address_already_in_use: return "Address already in use";
        case errors::can_not_assign_requested_address: return "Cannot assign requested address";
        case errors::network_is_down: return "Network is down";
        case errors::network_is_unreachable: return "Network is unreachable";
        case errors::network_reset: return "Network dropped connection because of reset";
        case errors::connection_aborted: return "Software caused connection abort";
        case errors::connection_reset_by_peer: return "Connection reset by peer";
        case errors::no_buffer_space_available: return "No buffer space available";
        case errors::transport_endpoint_already_connected: return "Transport endpoint is already connected";
        case errors::transport_endpoint_not_connected: return "Transport endpoint is not connected";
        case errors::can_not_send_after_transport_endpoint_shutdown: return "Cannot send after transport endpoint shutdown";
        case errors::too_many_references: return "Too many references: cannot splice";
        case errors::connection_timed_out: return "Connection timed out";
        case errors::connection_refused: return "Connection refused";
        case errors::host_is_down: return "Host is down";
        case errors::no_route_to_host: return "No route to host";
        case errors::operation_already_in_progress: return "Operation already in progress";
        case errors::operation_in_progress: return "Operation now in progress";
        case errors::stale_file_handle: return "Stale file handle";
        case errors::structure_needs_cleaning: return "Structure needs cleaning";
        case errors::not_a_xenix_named_type_file: return "Not a XENIX named type file";
        case errors::no_xenix_semaphores_available: return "No XENIX semaphores available";
        case errors::isa_named_type_file: return "Is a named type file";
        case errors::remote_io: return "Remote I/O error";
        case errors::quota_exceeded: return "Quota exceeded";
        case errors::no_medium_found: return "No medium found";
        case errors::bad_medium_type: return "Wrong medium type";
        case errors::operation_canceled: return "Operation Canceled";
        case errors::no_key: return "Required key not available";
        case errors::key_expired: return "Key has expired";
        case errors::key_revoked: return "Key has been revoked";
        case errors::key_rejected: return "Key was rejected by service";
        case errors::owner_died: return "Owner died";
        case errors::state_not_recoverable: return "State not recoverable";
        case errors::rf_kill: return "Operation not possible due to RF-kill";
        case errors::memory_page_has_hardware_error: return "Memory page has hardware error";
        default: return "Unknown";
    }
}
