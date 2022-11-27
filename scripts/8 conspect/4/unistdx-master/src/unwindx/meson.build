unwindx_cpp_args = ['-DUNW_LOCAL_ONLY']
unwindx_deps = [dependency('libunwind')]
unwindx_src = files([
    'unwind.cc'
])
unwindx_lib = library(
    'unwindx',
    sources: unwindx_src,
    version: meson.project_version(),
    install: true,
    include_directories: src,
    implicit_include_directories: false,
    dependencies: unwindx_deps,
    cpp_args: unwindx_cpp_args,
)

pkgconfig.generate(
    unwindx_lib,
    requires: unwindx_deps,
    version: meson.project_version(),
    name: 'unwindx',
    description: 'C++ wrapper for libunwind',
    extra_cflags: unwindx_cpp_args,
)

unwindx = declare_dependency(
    link_with: unwindx_lib,
    include_directories: src,
    dependencies: unwindx_deps,
)

install_headers('unwind', subdir: 'unwindx')
