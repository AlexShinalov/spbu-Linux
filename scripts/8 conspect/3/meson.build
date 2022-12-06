project(
  'myproject',
  'cpp',
  version: '0.1.0',
  meson_version: '>=0.50',
  default_options: ['cpp_std=c++11']
)


myproject_src = files([
    'main.cc'
])

zlib = dependency('zlib')

executable(
    'myproject',
    include_directories: [],
    sources: myproject_src,
    dependencies: [zlib],
    install: true
)
