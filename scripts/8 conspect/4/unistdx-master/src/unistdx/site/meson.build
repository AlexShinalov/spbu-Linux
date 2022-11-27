guile = find_program('guile')
if guile.found()
    run_target('site',
              command: [guile, '-s', join_paths(meson.current_source_dir(), 'site.scm'),
                        '--doxygen', xml_output_dir,
                        '--doxyfile', doxyfile_path,
                        '--output', meson.current_build_dir()],
              depends: doxygen_xml)
endif

curl = find_program('curl', required: false)
if curl.found()
    url = 'https://mirror.cmmshq.ru/bootstrap.min.css'
    file = join_paths(meson.current_build_dir(), 'bootstrap.min.css')
    run_target('bootstrap-css', command: [curl, '-s', '-o', file, url])
endif

#configure_file(input: 'scheme.css', output: 'scheme.css', copy: true)
