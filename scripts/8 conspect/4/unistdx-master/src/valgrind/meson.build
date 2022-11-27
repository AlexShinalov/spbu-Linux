valgrind_conf = configuration_data()
valgrind_dep = dependency('valgrind', required: false)
if valgrind_dep.found()
    valgrind_dep = [valgrind_dep]
    valgrind_conf.set('UNISTDX_TEST_HAVE_VALGRIND_H', true)
else
    valgrind_dep = []
endif
configure_file(
    input: 'config.in',
    output: 'config',
    configuration: valgrind_conf
)
