project(
    'unistdx',
    'cpp',
    version: '0.70.3',
    meson_version: '>=0.47',
    default_options: ['cpp_std=c++11'],
    license: ['Public domain']
)

with_tests = get_option('with_tests')
with_site = get_option('with_site')
with_backtrace_on_exception = get_option('with_backtrace_on_exception')
with_libdw = get_option('with_libdw')

cpp = meson.get_compiler('cpp')
cpp_args = []
cpp_linker_args = []
pkgconfig = import('pkgconfig')
suffix = ''

if with_backtrace_on_exception
    cpp_args += '-DUNISTDX_DEBUG'
    cpp_args += '-DUNISTDX_ENABLE_BAD_CALL_BACKTRACE'
    #cpp_args += '-DUNW_LOCAL_ONLY'
    suffix = '-debug'
endif

if with_libdw
    cpp_args += '-DUNISTDX_WITH_LIBDW'
endif

foreach arg : [
    '-Werror=return-type',
    '-Werror=return-local-addr',
    '-Werror=cast-align',
    '-Wcast-align=strict',
    '-Wno-terminate',
    '-Wfloat-conversion',
    '-Werror=float-conversion',
    '-fvisibility-inlines-hidden',
]
    if cpp.has_argument(arg)
        add_global_arguments(arg, language: 'cpp')
    endif
endforeach

add_global_arguments(cpp_args, language: 'cpp')

# compile-time configuration {{{
all_defines = []
config = configuration_data()
foreach pair : [
    ['bswap16', '0'],
    ['bswap32', '0'],
    ['bswap64', '0'],
    ['clz', '1'],
    ['clzl', '1'],
    ['clzll', '1'],
    ['popcount', '0'],
    ['popcountl', '0'],
    ['popcountll', '0'],
    ['unreachable', ''],
]
    func = pair[0]
    args = pair[1]
    builtin = 'int main() { @0@(@1@); }'
    all_defines += 'UNISTDX_HAVE_BUILTIN_' + func.to_upper()
    if cpp.compiles(builtin.format('__builtin_' + func, args))
        config.set(all_defines[-1], true)
    endif
endforeach
if host_machine.endian() == 'little'
    all_defines += 'UNISTDX_LITTLE_ENDIAN'
    config.set(all_defines[-1], true)
elif host_machine.endian() == 'big'
    all_defines += 'UNISTDX_BIG_ENDIAN'
    config.set(all_defines[-1], true)
else
    error('unknown endiannes')
endif
foreach pair : [
    ['__int128', 'int128'],
    ['long double'],
    ['long long'],
]
    type = pair[0]
    name = type.underscorify()
    if pair.length() > 1
        name = pair[1]
    endif
    all_defines += 'UNISTDX_HAVE_' + name.to_upper()
    if cpp.has_type(type)
        config.set(all_defines[-1], true)
        config.set('UNISTDX_SIZE_OF_' + name.to_upper(), cpp.sizeof(type))
    endif
endforeach
foreach pair : [
    ['dirent.h', 'DTTOIF'],
    ['dlfcn.h', 'dladdr'],
    ['execinfo.h', 'backtrace'],
    ['fcntl.h', 'F_GETPIPE_SZ'],
    ['fcntl.h', 'F_SETNOSIGPIPE'],
    ['fcntl.h', 'F_SETPIPE_SZ'],
    ['fcntl.h', 'O_CLOEXEC'],
    ['fcntl.h', 'O_DIRECT'],
    ['fcntl.h', 'O_DIRECTORY'],
    ['fcntl.h', 'O_LARGEFILE'],
    ['fcntl.h', 'O_NOATIME'],
    ['fcntl.h', 'O_NOFOLLOW'],
    ['fcntl.h', 'O_PATH'],
    ['fcntl.h', 'O_RSYNC'],
    ['fcntl.h', 'O_TMPFILE'],
    ['fcntl.h', 'SPLICE_F_GIFT'],
    ['fcntl.h', 'SPLICE_F_MORE'],
    ['fcntl.h', 'SPLICE_F_MOVE'],
    ['fcntl.h', 'SPLICE_F_NONBLOCK'],
    ['fcntl.h', 'splice'],
    ['grp.h', 'getgrgid_r'],
    ['grp.h', 'getgrnam_r'],
    ['link.h', 'dl_iterate_phdr'],
    ['linux/sockios.h', 'SIOCBRADDBR'],
    ['linux/sockios.h', 'SIOCBRADDIF'],
    ['linux/sockios.h', 'SIOCBRDELBR'],
    ['linux/sockios.h', 'SIOCBRDELIF'],
    ['net/if.h', 'IFF_ALLMULTI'],
    ['net/if.h', 'IFF_BROADCAST'],
    ['net/if.h', 'IFF_DEBUG'],
    ['net/if.h', 'IFF_LOOPBACK'],
    ['net/if.h', 'IFF_MASTER'],
    ['net/if.h', 'IFF_MULTICAST'],
    ['net/if.h', 'IFF_NOARP'],
    ['net/if.h', 'IFF_NOTRAILERS'],
    ['net/if.h', 'IFF_POINTOPOINT'],
    ['net/if.h', 'IFF_PORTSEL'],
    ['net/if.h', 'IFF_PROMISC'],
    ['net/if.h', 'IFF_RUNNING'],
    ['net/if.h', 'IFF_SLAVE'],
    ['net/if.h', 'IFF_UP'],
    ['netinet/tcp.h', 'TCP_USER_TIMEOUT'],
    ['poll.h', 'POLLRDHUP'],
    ['pwd.h', 'getpwnam_r'],
    ['pwd.h', 'getpwuid_r'],
    ['sched.h', 'CLONE_FILES'],
    ['sched.h', 'CLONE_FS'],
    ['sched.h', 'CLONE_NEWCGROUP'],
    ['sched.h', 'CLONE_NEWIPC'],
    ['sched.h', 'CLONE_NEWNET'],
    ['sched.h', 'CLONE_NEWNS'],
    ['sched.h', 'CLONE_NEWPID'],
    ['sched.h', 'CLONE_NEWUSER'],
    ['sched.h', 'CLONE_NEWUTS'],
    ['sched.h', 'CLONE_SYSVSEM'],
    ['sched.h', 'clone'],
    ['sched.h', 'setns'],
    ['sched.h', 'unshare'],
    ['semaphore.h', 'sem_timedwait'],
    ['signal.h', 'SIGPOLL'],
    ['signal.h', 'SIGPWR'],
    ['signal.h', 'SIGSTKFLT'],
    ['signal.h', 'SIGWINCH'],
    ['stdlib.h', 'mkostemp'],
    ['stdlib.h', 'mkstemp'],
    ['sys/eventfd.h', 'eventfd'],
    ['sys/ioctl.h', 'FIONREAD'],
    ['sys/ioctl.h', 'SIOCADDMULTI'],
    ['sys/ioctl.h', 'SIOCDELMULTI'],
    ['sys/ioctl.h', 'SIOCDIFADDR'],
    ['sys/ioctl.h', 'SIOCGIFADDR'],
    ['sys/ioctl.h', 'SIOCGIFBRDADDR'],
    ['sys/ioctl.h', 'SIOCGIFCONF'],
    ['sys/ioctl.h', 'SIOCGIFCOUNT'],
    ['sys/ioctl.h', 'SIOCGIFDSTADDR'],
    ['sys/ioctl.h', 'SIOCGIFENCAP'],
    ['sys/ioctl.h', 'SIOCGIFFLAGS'],
    ['sys/ioctl.h', 'SIOCGIFFLAGS'],
    ['sys/ioctl.h', 'SIOCGIFHWADDR'],
    ['sys/ioctl.h', 'SIOCGIFINDEX'],
    ['sys/ioctl.h', 'SIOCGIFMEM'],
    ['sys/ioctl.h', 'SIOCGIFMETRIC'],
    ['sys/ioctl.h', 'SIOCGIFMTU'],
    ['sys/ioctl.h', 'SIOCGIFNAME'],
    ['sys/ioctl.h', 'SIOCGIFNETMASK'],
    ['sys/ioctl.h', 'SIOCGIFPFLAGS'],
    ['sys/ioctl.h', 'SIOCGIFSLAVE'],
    ['sys/ioctl.h', 'SIOCSIFADDR'],
    ['sys/ioctl.h', 'SIOCSIFBRDADDR'],
    ['sys/ioctl.h', 'SIOCSIFDSTADDR'],
    ['sys/ioctl.h', 'SIOCSIFENCAP'],
    ['sys/ioctl.h', 'SIOCSIFFLAGS'],
    ['sys/ioctl.h', 'SIOCSIFFLAGS'],
    ['sys/ioctl.h', 'SIOCSIFHWADDR'],
    ['sys/ioctl.h', 'SIOCSIFHWBROADCAST'],
    ['sys/ioctl.h', 'SIOCSIFLINK'],
    ['sys/ioctl.h', 'SIOCSIFMEM'],
    ['sys/ioctl.h', 'SIOCSIFMETRIC'],
    ['sys/ioctl.h', 'SIOCSIFMTU'],
    ['sys/ioctl.h', 'SIOCSIFNAME'],
    ['sys/ioctl.h', 'SIOCSIFNETMASK'],
    ['sys/ioctl.h', 'SIOCSIFPFLAGS'],
    ['sys/ioctl.h', 'SIOCSIFSLAVE'],
    ['sys/ioctl.h', 'ioctl'],
    ['sys/mman.h', 'MADV_DODUMP'],
    ['sys/mman.h', 'MADV_DOFORK'],
    ['sys/mman.h', 'MADV_DONTDUMP'],
    ['sys/mman.h', 'MADV_DONTFORK'],
    ['sys/mman.h', 'MADV_FREE'],
    ['sys/mman.h', 'MADV_HUGEPAGE'],
    ['sys/mman.h', 'MADV_HWPOISON'],
    ['sys/mman.h', 'MADV_KEEPONFORK'],
    ['sys/mman.h', 'MADV_MERGEABLE'],
    ['sys/mman.h', 'MADV_NOHUGEPAGE'],
    ['sys/mman.h', 'MADV_REMOVE'],
    ['sys/mman.h', 'MADV_SOFT_OFFLINE'],
    ['sys/mman.h', 'MADV_UNMERGEABLE'],
    ['sys/mman.h', 'MADV_WIPEONFORK'],
    ['sys/mman.h', 'MAP_32BIT'],
    ['sys/mman.h', 'MAP_ANONYMOUS'],
    ['sys/mman.h', 'MAP_GROWSDOWN'],
    ['sys/mman.h', 'MAP_HUGETLB'],
    ['sys/mman.h', 'MAP_HUGE_1GB'],
    ['sys/mman.h', 'MAP_HUGE_2MB'],
    ['sys/mman.h', 'MAP_LOCKED'],
    ['sys/mman.h', 'MAP_NONBLOCK'],
    ['sys/mman.h', 'MAP_NORESERVE'],
    ['sys/mman.h', 'MAP_POPULATE'],
    ['sys/mman.h', 'MAP_STACK'],
    ['sys/mman.h', 'MAP_UNINITIALIZED'],
    ['sys/mman.h', 'mmap'],
    ['sys/mount.h', 'mount'],
    ['sys/prctl.h', 'PR_GET_NO_NEW_PRIVS'],
    ['sys/prctl.h', 'PR_SET_NO_NEW_PRIVS'],
    ['sys/prctl.h', 'prctl'],
    ['sys/resource.h', 'getrusage'],
    ['sys/sem.h', 'semtimedop'],
    ['sys/sendfile.h', 'sendfile'],
    ['sys/shm.h', 'SHM_HUGETLB'],
    ['sys/shm.h', 'SHM_HUGE_1GB'],
    ['sys/shm.h', 'SHM_HUGE_2MB'],
    ['sys/shm.h', 'SHM_HUGE_SHIFT'],
    ['sys/shm.h', 'SHM_NORESERVE'],
    ['sys/socket.h', 'SCM_CREDENTIALS'],
    ['sys/socket.h', 'SCM_RIGHTS'],
    ['sys/socket.h', 'SOCK_CLOEXEC'],
    ['sys/socket.h', 'SOCK_NONBLOCK'],
    ['sys/socket.h', 'SO_PEERCRED'],
    ['sys/socket.h', 'accept4'],
    ['sys/statfs.h', 'statfs'],
    ['sys/statvfs.h', 'statvfs'],
    ['sys/sysinfo.h', 'sysinfo'],
    ['unistd.h', 'SEEK_DATA'],
    ['unistd.h', 'SEEK_HOLE'],
    ['unistd.h', 'copy_file_range'],
    ['unistd.h', 'dup3'],
    ['unistd.h', 'pipe2'],
]
    header = pair[0]
    symbol = pair[1]
    all_defines += 'UNISTDX_HAVE_' + symbol.to_upper()
    if cpp.has_header_symbol(header, symbol)
        config.set(all_defines[-1], true)
    endif
endforeach
foreach header : [
    'cxxabi.h',
    'dlfcn.h',
    'ifaddrs.h',
    'linux/capability.h',
    'linux/netlink.h',
    'linux/seccomp.h',
    'linux/securebits.h',
    'linux/sockios.h',
    'netdb.h',
    'netinet/tcp.h',
    'sched.h',
    'semaphore.h',
    'sys/epoll.h',
    'sys/ioctl.h',
    'sys/resource.h',
    'sys/sem.h',
    'sys/time.h',
    'sys/xattr.h',
    'syslog.h',
    'ucontext.h',
]
    all_defines += 'UNISTDX_HAVE_' + header.underscorify().to_upper()
    if cpp.has_header(header)
        config.set(all_defines[-1], true)
    endif
endforeach
all_defines += 'UNISTDX_HAVE_GECOS'
if cpp.has_member('passwd', 'pw_gecos', prefix: '#include <pwd.h>')
    config.set('UNISTDX_HAVE_GECOS', true)
endif
if config.has('UNISTDX_HAVE_BACKTRACE')
    if cpp.get_id() == 'gcc' or cpp.has_link_argument('-rdynamic')
        add_global_link_arguments('-rdynamic', language: 'cpp')
    endif
endif
unistdx_version = get_option('buildtype').contains('debug') ? '0.0.0' : meson.project_version()
config.set('UNISTDX_VERSION', unistdx_version)
v = unistdx_version.split('.')
config.set('UNISTDX_MAJOR', v[0])
config.set('UNISTDX_MINOR', v[1])
config.set('UNISTDX_PATCHLEVEL', v[2])
# }}}

threads = dependency('threads')

subdir('src')
subdir('ci')

# cppcheck {{{
cppcheck = find_program('cppcheck', required: false)
if cppcheck.found()
    run_target(
        'cppcheck',
        command: [
            cppcheck,
            '--enable=all',
            '--quiet',
            '--force',
            '--language=c++',
            '--std=c++11',
            '--template=gcc',
            '-I' + join_paths(meson.source_root(), 'src'),
            '-I' + join_paths(meson.build_root(), 'src'),
            join_paths(meson.source_root(), 'src'),
        ]
    )
endif
# }}}
# clang-check {{{
clang_check = find_program('clang-check', required: false)
if clang_check.found()
    run_target(
        'clang-check',
        command: [clang_check, '-analyze'] + libunistdx_src + [
            '--',
            '-I' + join_paths(meson.source_root(), 'src'),
            '-I' + join_paths(meson.build_root(), 'src'),
            '-std=c++11'
        ]
    )
endif
# }}}
