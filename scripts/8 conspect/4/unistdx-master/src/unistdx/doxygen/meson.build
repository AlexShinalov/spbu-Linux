# doxygen tag files {{{
std_tagfile_name = 'cppreference-doxygen-web.tag.xml'
std_tagfile_prefix = 'http://upload.cppreference.com/mwiki/images/f/f8/'
std_tagfile_url = std_tagfile_prefix + std_tagfile_name
std_tag_xml = join_paths(meson.build_root(), 'std.tag.xml')
std_tag_url = 'http://en.cppreference.com/w/'
wget = find_program('wget', required: false)
curl = find_program('curl', required: false)
if wget.found()
    run_target(
        'doxygen-std-tagfile',
        command: [wget, '-O', std_tag_xml, std_tagfile_url]
    )
elif curl.found()
    run_target(
        'doxygen-std-tagfile',
        command: [curl, '-v', '-o', std_tag_xml, std_tagfile_url]
    )
endif
# }}}
# generate Doxyfile {{{
csd = meson.current_source_dir()
cbd = meson.current_build_dir()
html_output_dir = join_paths(meson.build_root(), 'html')
doxyfile = configuration_data()
doxyfile.set('LAYOUT_FILE', join_paths(csd, 'layout.xml'))
doxyfile.set('OUTPUT_DIRECTORY', meson.build_root())
doxyfile.set('TAGFILES', std_tag_xml + '=' + std_tag_url)
doxyfile.set('PREDEFINED', ' '.join(all_defines))
excluded_files = [
    'meson.build',
    join_paths(meson.source_root(), 'src', 'unistdx', 'bits'),
    join_paths(meson.source_root(), 'src', 'unistdx', 'test'),
    join_paths(meson.source_root(), 'src', 'unistdx', 'doxygen'),
    meson.build_root(),
]
doxyfile.set('EXCLUDE', ' '.join(excluded_files))
doxyfile.set('EXCLUDE_PATTERNS', ' '.join(['*.css', '*.scm', '*_test.cc', 'meson.build',
                                           '*~', '.*', '*_benchmark.cc']))
doxyfile.set('EXCLUDE_SYMBOLS', ' '.join(['bits']))
input_files = [join_paths(meson.source_root(), 'src', 'unistdx')]
doxyfile.set('INPUT', ' '.join(input_files))
dot = find_program('dot', required: false)
if dot.found()
    doxyfile.set('DOT_PATH', dot.path())
    doxyfile.set('HAVE_DOT', 'YES')
else
    doxyfile.set('HAVE_DOT', 'NO')
    doxyfile.set('DOT_PATH', '')
endif
xml_output_dir = meson.current_build_dir()
doxyfile.set('XML_OUTPUT', xml_output_dir)
configure_file(
    input: 'Doxyfile.in',
    output: 'Doxyfile',
    configuration: doxyfile
)
doxyfile_path = join_paths(meson.current_build_dir(), 'Doxyfile')
# }}}

doxygen = find_program('doxygen')
sh = find_program('sh', required: false)
if doxygen.found() and sh.found()
    # N.B. Doxygen is sensitive to the working directory if you use pages.
    # Always run it in the current source directory.
    doxygen_xml = [custom_target(
        'doxygen',
        command: [
            sh,
            '-c',
            ' && '.join([
                ' '.join(['cd', meson.current_source_dir()]),
                ' '.join([doxygen.path(), join_paths(meson.current_build_dir(), 'Doxyfile')])
            ])
        ],
        output: 'index.xml',
        build_always_stale: true
    )]
else
    doxygen_xml = []
endif
