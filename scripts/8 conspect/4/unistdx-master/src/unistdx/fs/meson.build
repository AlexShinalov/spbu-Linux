libunistdx_src += files([
    'copy_file.cc',
    'file_mode.cc',
    'file_mutex.cc',
    'file_status.cc',
    'file_type.cc',
    'mkdirs.cc',
    'odirtree.cc',
    'path.cc',
    'path_view.cc',
    'temporary_file.cc',
])

install_headers(
    'canonical_path',
    'path_view',
    'copy_file',
    'directory',
    'directory_entry',
    'dirstream',
    'file_attributes',
    'file_mode',
    'file_mutex',
    'file_status',
    'file_system_status',
    'file_type',
    'idirectory',
    'idirtree',
    'mkdirs',
    'odirectory',
    'odirtree',
    'path',
    'path_flag',
    'temporary_file',
    subdir: join_paths(meson.project_name(), 'fs')
)

libunistdx_tests_with_stubs += [
    [files('copy_file_test.cc'),
    [[],
    [copy_file_range_stub],
    [copy_file_range_stub, sendfile_stub]]],
]

libunistdx_tests += files([
    'canonical_path_test.cc',
    'file_attributes_test.cc',
    'file_mode_test.cc',
    'file_mutex_test.cc',
    'file_status_test.cc',
    'file_system_status_test.cc',
    'file_type_test.cc',
    'idirectory_test.cc',
    'idirtree_test.cc',
    'mkdirs_test.cc',
    'path_test.cc',
    ])
