libunistdx_src += files([
    'bad_call.cc',
    'base64.cc',
    'byte_buffer.cc',
    'command_line.cc',
    'sha1.cc',
    'sha2.cc',
    'string.cc',
    'uint128.cc',
    'websocketbuf.cc',
    'websocket.cc',
])

install_headers(
    'array_view',
    'bad_call',
    'base64',
    'byte_buffer',
    'check',
    'command_line',
    'contracts',
    'delete_each',
    'flag',
    'ios_guard',
    'log_message',
    'make_object',
    'packetbuf',
    'recursive_spin_mutex',
    'sha1',
    'sha2',
    'simple_lock',
    'spin_mutex',
    'streambuf_traits',
    'string',
    'types',
    'uint128',
    'unlock_guard',
    'websocket',
    'websocketbuf',
    subdir: join_paths(meson.project_name(), 'base')
)

libunistdx_tests += files([
    'bad_call_test.cc',
    'byte_buffer_test.cc',
    'command_line_test.cc',
    'delete_each_test.cc',
    'flag_test.cc',
    'log_message_test.cc',
    'sha1_test.cc',
    'sha2_test.cc',
    'spin_mutex_test.cc',
    'string_test.cc',
    'uint128_test.cc',
    'websocket_test.cc',
    ])

libunistdx_benchmarks += files([
    'sha1_benchmark.cc',
])
