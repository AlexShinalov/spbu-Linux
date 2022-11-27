# clang-tidy {{{
clang_tidy = find_program('clang-tidy', required: false)
guix = find_program('guix', required: false)
if clang_tidy.found() and guix.found()
    clang_tidy_checks = [
        '-*',
        'bugprone-*',
        '-bugprone-unused-raii',
        '-bugprone-narrowing-conversions',
        '-bugprone-exception-escape',
        'cppcoreguidelines-avoid-goto',
        'cppcoreguidelines-slicing',
        'cppcoreguidelines-special-member-functions',
        'misc-*',
        '-misc-non-private-member-variables-in-classes',
        'performance-*',
        '-performance-unnecessary-value-param',
    ]
    run_target(
        'clang-tidy',
        command: [
            guix,
            'repl',
            '--',
            join_paths(meson.current_source_dir(), 'clang-tidy.scm'),
            clang_tidy.path(),
            '-p=' + meson.build_root(),
            '--checks=' + ','.join(clang_tidy_checks),
            '--warnings-as-errors=*',
            '--quiet',
        ] + libunistdx_src + [
            '--',
            '-I' + join_paths(meson.source_root(), 'src'),
            '-I' + join_paths(meson.build_root(), 'src'),
            '-std=c++11'
        ]
    )
endif
# }}}
# valgrind {{{
valgrind = find_program('valgrind', required: false)
guile = find_program('guile', required: false)
if valgrind.found() and guile.found()
    run_target(
        'valgrind',
        command: [guile, join_paths(meson.current_source_dir(), 'valgrind.scm')]
    )
endif
# }}}
