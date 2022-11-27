libunistdx_src += files([
    'epoll_event.cc',
    'fildes.cc',
    'pipe.cc',
    'poll_event.cc',
    'shared_byte_buffer.cc',
    'two_way_pipe.cc',
])

install_headers(
    'epoll_event',
    'event_file_descriptor',
    'fd_type',
    'fdstream',
    'fildes',
    'fildes_pair',
    'fildesbuf',
    'memory_mapping',
    'open_flag',
    'pipe',
    'poll_event',
    'poller',
    'shared_byte_buffer',
    'sysstream',
    'terminal',
    'two_way_pipe',
    subdir: join_paths(meson.project_name(), 'io')
)

libunistdx_tests += files([
    'epoll_event_test.cc',
    'fildes_test.cc',
    'fildesbuf_test.cc',
    'memory_mapping_test.cc',
    'pipe_test.cc',
    'poll_event_test.cc',
    'poller_test.cc',
    'shared_byte_buffer_test.cc',
    'two_way_pipe_test.cc',
    ])


libunistdx_benchmarks += files([
    'memory_mapping_benchmark.cc',
])
