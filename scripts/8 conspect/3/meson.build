project(
    'myproject',                       # название проекта
    'cpp',                             # язык
    version: '0.1.0',                  # версия кода
    meson_version: '>=0.50',           # минимально поддерживаемая версия Meson
    default_options: ['cpp_std=c++11'] # используемый стандарт C++
)
myproject_src = files([
    'main.cpp' # список исходных файлов
])

zlib = dependency('zlib') # подключаем zlib

executable(
    'myproject',              # название исполняемого файла
    include_directories: [], # где ищутся заголовочные файлы
    sources: myproject_src,   # список исходный файлов
    dependencies: [zlib],         # зависимости проекта (если имеются)
    install: true             # устанавливать ли файл
)
