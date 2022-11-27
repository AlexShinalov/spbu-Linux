configure_file(
    input: 'config.in',
    output: 'config',
    configuration: config,
    install_dir: join_paths(
        get_option('prefix'),
        get_option('includedir'),
        meson.project_name()
    )
)

install_headers('forward', subdir: meson.project_name())

libunistdx_src = files()
libunistdx_tests = files()
libunistdx_tests_with_stubs = files()
libunistdx_benchmarks = files()
libunistdx_test_src = files()

subdir('stub')
subdir('base')
subdir('fs')
subdir('it')
subdir('net')
subdir('io')
subdir('ipc')
subdir('util')
subdir('system')
subdir('bits')
subdir('test')

unistdx_deps = []
unistdx_libs = []
if with_libdw
    unistdx_deps += dependency('libdw')
endif
if libdl.found()
    unistdx_libs += libdl
endif

unistdx_lib = library(
    'unistdx' + suffix,
    sources: libunistdx_src,
    version: meson.project_version(),
    install: true,
    include_directories: src,
    implicit_include_directories: false,
    dependencies: unistdx_deps + unistdx_libs,
    cpp_args: cpp_args,
    link_args: cpp_linker_args,
)

pkgconfig.generate(
    unistdx_lib,
    libraries: unistdx_libs,
    requires: unistdx_deps,
    version: meson.project_version(),
    name: 'unistdx' + suffix,
    description: 'C++ wrappers for libc',
    extra_cflags: cpp_args,
)

unistdx = declare_dependency(
    link_with: unistdx_lib,
    include_directories: src,
    dependencies: unistdx_deps + unistdx_libs,
)

unistdx_test_deps = [threads]
unistdx_test_libs = [unistdx]

unistdx_test_lib = library(
    'unistdx-test',
    sources: libunistdx_test_src,
    version: meson.project_version(),
    install: true,
    include_directories: src,
    implicit_include_directories: false,
    dependencies: unistdx_test_deps + unistdx_test_libs,
    cpp_args: cpp_args,
    link_args: cpp_linker_args,
)

pkgconfig.generate(
    unistdx_test_lib,
    libraries: unistdx_test_libs,
    requires: unistdx_test_deps,
    version: meson.project_version(),
    name: 'unistdx-test',
    description: 'Process-based unit tests',
    extra_cflags: cpp_args,
)

unistdx_test = declare_dependency(
    link_with: unistdx_test_lib,
    include_directories: src,
    dependencies: unistdx_test_deps + unistdx_test_libs,
)

if with_tests
    test(
        'no-stubs',
        executable(
            'no-stubs',
            sources: libunistdx_tests,
            include_directories: src,
            implicit_include_directories: false,
            dependencies: [unistdx_test,threads] + (valgrind_dep.found() ? [valgrind_dep] : []) + unistdx_deps + unistdx_libs,
            link_with: [unistdx_test_lib,unistdx_lib],
        ),
        is_parallel: false,
        args: ['unshare=']
    )
    foreach tuple : libunistdx_tests_with_stubs
        file = tuple[0]
        stubs = tuple.length() == 1 ? [[]] : tuple[1]
        arguments = tuple.length() <= 2 ? [[]] : tuple[2]
        path = '@0@'.format(file)
        basename = path.split('/')[-1]
        dirname = path.split('/')[-2]
        words = []
        foreach w : basename.split('.')[0].split('_')
            if w != 'test'
                words += w
            endif
        endforeach
        name = '_'.join(words)
        test_name = dirname + '/' + name
        source_name = file
        foreach libs : stubs
            suffix = ''
            foreach l : libs
                tmp = l.full_path().split('lib')
                suffix += '--' + tmp[tmp.length()-1].split('.so')[0]
            endforeach
            exe = executable(
                '_'.join(words) + '_test' + suffix.underscorify(),
                sources: source_name,
                include_directories: src,
                dependencies: [unistdx_test,threads] + (valgrind_dep.found() ? [valgrind_dep] : []) + unistdx_deps + unistdx_libs,
                link_with: libs,
                implicit_include_directories: false,
            )
            foreach args : arguments
                tmp = test_name + suffix
                if args.length() != 0
                    tmp += '--' + '_'.join(args)
                endif
                test(tmp, exe, args: args + ['unshare='], is_parallel: false)
            endforeach
        endforeach
    endforeach
endif

foreach file : libunistdx_benchmarks
    path = '@0@'.format(file)
    basename = path.split('/')[-1]
    dirname = path.split('/')[-2]
    words = []
    foreach w : basename.split('.')[0].split('_')
        if w != 'benchmark'
            words += w
        endif
    endforeach
    name = '_'.join(words)
    benchmark_name = dirname + '/' + name
    source_name = file
    benchmark(
        benchmark_name,
        executable(
            '_'.join(words) + '_benchmark',
            sources: source_name,
            include_directories: src,
            dependencies: [unistdx] + unistdx_deps + unistdx_libs,
            implicit_include_directories: false,
        )
    )
endforeach

executable(
    'colors',
    sources: 'test/colors.cc',
    include_directories: src,
    implicit_include_directories: false,
    dependencies: [unistdx],
)

executable(
    'pivot-root-test',
    sources: 'test/pivot_root_test.cc',
    include_directories: src,
    implicit_include_directories: false,
    dependencies: [unistdx],
    cpp_args: cpp_args,
    link_args: cpp_linker_args,
)

if with_site
    subdir('doxygen')
    subdir('site')
endif
