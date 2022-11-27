libunistdx_src += files([
    'error.cc',
    'nss.cc',
    'resource.cc',
    'security.cc',
])

libdl = cpp.find_library('dl', required: false)
if libdl.found()
    libunistdx_src += files(['linker.cc'])
endif

libunistdx_tests += files([
    'log_test.cc',
    'resource_test.cc',
    'security_test.cc',
    'clock_test.cc',
    'error_test.cc'
])

libunistdx_tests_with_stubs += [
    [files('users_test.cc'), [[], [sysconf_stub]]],
]

if config.get('UNISTDX_HAVE_BACKTRACE', false)
    libunistdx_tests_with_stubs += [
        [files('backtrace_test.cc'),
         [[], [backtrace_symbols_stub], [backtrace_symbols_stub_2]]],
    ]
endif

install_headers(
    'call',
    'clock',
    'error',
    'linker',
    'log',
    'nss',
    'resource',
    'security',
    'time',
    subdir: join_paths(meson.project_name(), 'system')
)
