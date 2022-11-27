dwx_cpp_args = []
dwx_deps = [dependency('libdw')]
dwx_src = files([
    'dw.cc'
])
dwx_lib = library(
    'dwx',
    sources: dwx_src,
    version: meson.project_version(),
    install: true,
    include_directories: src,
    implicit_include_directories: false,
    dependencies: dwx_deps,
    cpp_args: dwx_cpp_args,
)

pkgconfig.generate(
    dwx_lib,
    requires: dwx_deps,
    version: meson.project_version(),
    name: 'dwx',
    description: 'C++ wrapper for libunwind',
    extra_cflags: dwx_cpp_args,
)

dwx = declare_dependency(
    link_with: dwx_lib,
    include_directories: src,
    dependencies: dwx_deps,
)

install_headers('dw', subdir: 'dwx')
