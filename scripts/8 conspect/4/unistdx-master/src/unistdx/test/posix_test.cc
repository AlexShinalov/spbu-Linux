/*
UNISTDX — C++ library for Linux system calls.
© 2016, 2017, 2018, 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <iomanip>
#include <iostream>
#include <string>

#include <unistd.h>

#include <unistdx/test/color>

struct support {
unsigned int num_supported = 0;
unsigned int num_unsupported = 0;
unsigned int num_semisupported = 0;
};

support posix_1;
support posix_2;


void
printval(const std::string& name) {
    std::cout << std::setw(40) << std::left
        << name << Color::FG_LIGHT_RED;
    if (name.find("SOURCE") == std::string::npos) {
        if (name.find("POSIX2") != std::string::npos) {
            ++posix_2.num_unsupported;
        } else {
            ++posix_1.num_unsupported;
        }
        std::cout << "not supported";
    } else {
        std::cout << "undefined";
    }
    std::cout << Color::RESET << '\n';
}

void
might_be_supported(const std::string& name) {
    if (name.find("POSIX2") != std::string::npos) {
        ++posix_2.num_semisupported;
    } else {
        ++posix_1.num_semisupported;
    }
    std::cout << std::setw(40) << std::left << name
        << Color::FG_LIGHT_YELLOW
        << "might or might not be supported"
        << Color::RESET << '\n';
}

template<class T>
void
printval(const std::string& name, T val) {
    if (val == -1) {
        printval(name.c_str());
    } else if (val == 0) {
        might_be_supported(name);
    } else {
        std::cout << std::setw(40) << std::left << name;
        if (name.find("VERSION") != std::string::npos) {
            std::cout << val;
        } else {
            std::cout << Color::FG_LIGHT_GREEN;
            if (name.find("SOURCE") != std::string::npos) {
                std::cout << "defined";
            } else {
                std::cout << "supported";
                if (name.find("POSIX2") != std::string::npos) {
                    ++posix_2.num_supported;
                } else {
                    ++posix_1.num_supported;
                }
            }
            std::cout << Color::RESET;
            if (val != 1) {
                std::cout << " (" << val << ')';
            }
        }
        std::cout << '\n';
    }
}

void
ruler() {
std::cout << std::setw(40 + 31) << std::setfill('-')
    << "" << '\n' << std::setfill(' ');
}

unsigned int
posix_support(support s) {
    return static_cast<unsigned int>(100.f
        * (float(s.num_supported)
        / float(s.num_supported + s.num_unsupported + s.num_semisupported)));
}

void
summary() {
    ruler();
    std::cout << std::setw(40) << std::left
        << "POSIX1 support: "
        << posix_support(posix_1) << '%'
        << '\n';
    std::cout << std::setw(40) << std::left
        << "POSIX2 support: "
        << posix_support(posix_2) << '%'
        << '\n';
    ruler();
}

void test_posix() {
std::cout << std::setw(40) << std::left
    << "feature" << "status" << '\n';
ruler();
#if defined(_POSIX2_C_BIND)
printval("_POSIX2_C_BIND", _POSIX2_C_BIND);
#else
printval("_POSIX2_C_BIND");
#endif
#if defined(_POSIX2_C_DEV)
printval("_POSIX2_C_DEV", _POSIX2_C_DEV);
#else
printval("_POSIX2_C_DEV");
#endif
#if defined(_POSIX2_CHAR_TERM)
printval("_POSIX2_CHAR_TERM", _POSIX2_CHAR_TERM);
#else
printval("_POSIX2_CHAR_TERM");
#endif
#if defined(_POSIX2_FORT_DEV)
printval("_POSIX2_FORT_DEV", _POSIX2_FORT_DEV);
#else
printval("_POSIX2_FORT_DEV");
#endif
#if defined(_POSIX2_FORT_RUN)
printval("_POSIX2_FORT_RUN", _POSIX2_FORT_RUN);
#else
printval("_POSIX2_FORT_RUN");
#endif
#if defined(_POSIX2_LOCALEDEF)
printval("_POSIX2_LOCALEDEF", _POSIX2_LOCALEDEF);
#else
printval("_POSIX2_LOCALEDEF");
#endif
#if defined(_POSIX2_PBS)
printval("_POSIX2_PBS", _POSIX2_PBS);
#else
printval("_POSIX2_PBS");
#endif
#if defined(_POSIX2_PBS_ACCOUNTING)
printval("_POSIX2_PBS_ACCOUNTING", _POSIX2_PBS_ACCOUNTING);
#else
printval("_POSIX2_PBS_ACCOUNTING");
#endif
#if defined(_POSIX2_PBS_CHECKPOINT)
printval("_POSIX2_PBS_CHECKPOINT", _POSIX2_PBS_CHECKPOINT);
#else
printval("_POSIX2_PBS_CHECKPOINT");
#endif
#if defined(_POSIX2_PBS_LOCATE)
printval("_POSIX2_PBS_LOCATE", _POSIX2_PBS_LOCATE);
#else
printval("_POSIX2_PBS_LOCATE");
#endif
#if defined(_POSIX2_PBS_MESSAGE)
printval("_POSIX2_PBS_MESSAGE", _POSIX2_PBS_MESSAGE);
#else
printval("_POSIX2_PBS_MESSAGE");
#endif
#if defined(_POSIX2_PBS_TRACK)
printval("_POSIX2_PBS_TRACK", _POSIX2_PBS_TRACK);
#else
printval("_POSIX2_PBS_TRACK");
#endif
#if defined(_POSIX2_SW_DEV)
printval("_POSIX2_SW_DEV", _POSIX2_SW_DEV);
#else
printval("_POSIX2_SW_DEV");
#endif
#if defined(_POSIX2_SYMLINKS)
printval("_POSIX2_SYMLINKS", _POSIX2_SYMLINKS);
#else
printval("_POSIX2_SYMLINKS");
#endif
#if defined(_POSIX2_UPE)
printval("_POSIX2_UPE", _POSIX2_UPE);
#else
printval("_POSIX2_UPE");
#endif
#if defined(_POSIX_ADVISORY_INFO)
printval("_POSIX_ADVISORY_INFO", _POSIX_ADVISORY_INFO);
#else
printval("_POSIX_ADVISORY_INFO");
#endif
#if defined(_POSIX_ASYNCHRONOUS_IO)
printval("_POSIX_ASYNCHRONOUS_IO", _POSIX_ASYNCHRONOUS_IO);
#else
printval("_POSIX_ASYNCHRONOUS_IO");
#endif
#if defined(_POSIX_ASYNC_IO)
printval("_POSIX_ASYNC_IO", _POSIX_ASYNC_IO);
#else
printval("_POSIX_ASYNC_IO");
#endif
#if defined(_POSIX_BARRIERS)
printval("_POSIX_BARRIERS", _POSIX_BARRIERS);
#else
printval("_POSIX_BARRIERS");
#endif
#if defined(_POSIX_CHOWN_RESTRICTED)
printval("_POSIX_CHOWN_RESTRICTED", _POSIX_CHOWN_RESTRICTED);
#else
printval("_POSIX_CHOWN_RESTRICTED");
#endif
#if defined(_POSIX_CLOCK_SELECTION)
printval("_POSIX_CLOCK_SELECTION", _POSIX_CLOCK_SELECTION);
#else
printval("_POSIX_CLOCK_SELECTION");
#endif
#if defined(_POSIX_CPUTIME)
printval("_POSIX_CPUTIME", _POSIX_CPUTIME);
#else
printval("_POSIX_CPUTIME");
#endif
#if defined(_POSIX_FSYNC)
printval("_POSIX_FSYNC", _POSIX_FSYNC);
#else
printval("_POSIX_FSYNC");
#endif
#if defined(_POSIX_IPV6)
printval("_POSIX_IPV6", _POSIX_IPV6);
#else
printval("_POSIX_IPV6");
#endif
#if defined(_POSIX_JOB_CONTROL)
printval("_POSIX_JOB_CONTROL", _POSIX_JOB_CONTROL);
#else
printval("_POSIX_JOB_CONTROL");
#endif
#if defined(_POSIX_MAPPED_FILES)
printval("_POSIX_MAPPED_FILES", _POSIX_MAPPED_FILES);
#else
printval("_POSIX_MAPPED_FILES");
#endif
#if defined(_POSIX_MEMLOCK)
printval("_POSIX_MEMLOCK", _POSIX_MEMLOCK);
#else
printval("_POSIX_MEMLOCK");
#endif
#if defined(_POSIX_MEMLOCK_RANGE)
printval("_POSIX_MEMLOCK_RANGE", _POSIX_MEMLOCK_RANGE);
#else
printval("_POSIX_MEMLOCK_RANGE");
#endif
#if defined(_POSIX_MEMORY_PROTECTION)
printval("_POSIX_MEMORY_PROTECTION", _POSIX_MEMORY_PROTECTION);
#else
printval("_POSIX_MEMORY_PROTECTION");
#endif
#if defined(_POSIX_MESSAGE_PASSING)
printval("_POSIX_MESSAGE_PASSING", _POSIX_MESSAGE_PASSING);
#else
printval("_POSIX_MESSAGE_PASSING");
#endif
#if defined(_POSIX_MONOTONIC_CLOCK)
printval("_POSIX_MONOTONIC_CLOCK", _POSIX_MONOTONIC_CLOCK);
#else
printval("_POSIX_MONOTONIC_CLOCK");
#endif
#if defined(_POSIX_NO_TRUNC)
printval("_POSIX_NO_TRUNC", _POSIX_NO_TRUNC);
#else
printval("_POSIX_NO_TRUNC");
#endif
#if defined(_POSIX_PRIO_IO)
printval("_POSIX_PRIO_IO", _POSIX_PRIO_IO);
#else
printval("_POSIX_PRIO_IO");
#endif
#if defined(_POSIX_PRIORITIZED_IO)
printval("_POSIX_PRIORITIZED_IO", _POSIX_PRIORITIZED_IO);
#else
printval("_POSIX_PRIORITIZED_IO");
#endif
#if defined(_POSIX_PRIORITY_SCHEDULING)
printval("_POSIX_PRIORITY_SCHEDULING", _POSIX_PRIORITY_SCHEDULING);
#else
printval("_POSIX_PRIORITY_SCHEDULING");
#endif
#if defined(_POSIX_RAW_SOCKETS)
printval("_POSIX_RAW_SOCKETS", _POSIX_RAW_SOCKETS);
#else
printval("_POSIX_RAW_SOCKETS");
#endif
#if defined(_POSIX_READER_WRITER_LOCKS)
printval("_POSIX_READER_WRITER_LOCKS", _POSIX_READER_WRITER_LOCKS);
#else
printval("_POSIX_READER_WRITER_LOCKS");
#endif
#if defined(_POSIX_REALTIME_SIGNALS)
printval("_POSIX_REALTIME_SIGNALS", _POSIX_REALTIME_SIGNALS);
#else
printval("_POSIX_REALTIME_SIGNALS");
#endif
#if defined(_POSIX_REGEXP)
printval("_POSIX_REGEXP", _POSIX_REGEXP);
#else
printval("_POSIX_REGEXP");
#endif
#if defined(_POSIX_SAVED_IDS)
printval("_POSIX_SAVED_IDS", _POSIX_SAVED_IDS);
#else
printval("_POSIX_SAVED_IDS");
#endif
#if defined(_POSIX_SEMAPHORES)
printval("_POSIX_SEMAPHORES", _POSIX_SEMAPHORES);
#else
printval("_POSIX_SEMAPHORES");
#endif
#if defined(_POSIX_SHARED_MEMORY_OBJECTS)
printval("_POSIX_SHARED_MEMORY_OBJECTS", _POSIX_SHARED_MEMORY_OBJECTS);
#else
printval("_POSIX_SHARED_MEMORY_OBJECTS");
#endif
#if defined(_POSIX_SHELL)
printval("_POSIX_SHELL", _POSIX_SHELL);
#else
printval("_POSIX_SHELL");
#endif
#if defined(_POSIX_SPAWN)
printval("_POSIX_SPAWN", _POSIX_SPAWN);
#else
printval("_POSIX_SPAWN");
#endif
#if defined(_POSIX_SPIN_LOCKS)
printval("_POSIX_SPIN_LOCKS", _POSIX_SPIN_LOCKS);
#else
printval("_POSIX_SPIN_LOCKS");
#endif
#if defined(_POSIX_SPORADIC_SERVER)
printval("_POSIX_SPORADIC_SERVER", _POSIX_SPORADIC_SERVER);
#else
printval("_POSIX_SPORADIC_SERVER");
#endif
#if defined(_POSIX_SUB)
printval("_POSIX_SUB", _POSIX_SUB);
#else
printval("_POSIX_SUB");
#endif
#if defined(_POSIX_SUBPROFILE)
printval("_POSIX_SUBPROFILE", _POSIX_SUBPROFILE);
#else
printval("_POSIX_SUBPROFILE");
#endif
#if defined(_POSIX_SYNCHRONIZED_IO)
printval("_POSIX_SYNCHRONIZED_IO", _POSIX_SYNCHRONIZED_IO);
#else
printval("_POSIX_SYNCHRONIZED_IO");
#endif
#if defined(_POSIX_SYNC_IO)
printval("_POSIX_SYNC_IO", _POSIX_SYNC_IO);
#else
printval("_POSIX_SYNC_IO");
#endif
#if defined(_POSIX_THREAD_ATTR_STACKADDR)
printval("_POSIX_THREAD_ATTR_STACKADDR", _POSIX_THREAD_ATTR_STACKADDR);
#else
printval("_POSIX_THREAD_ATTR_STACKADDR");
#endif
#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
printval("_POSIX_THREAD_ATTR_STACKSIZE", _POSIX_THREAD_ATTR_STACKSIZE);
#else
printval("_POSIX_THREAD_ATTR_STACKSIZE");
#endif
#if defined(_POSIX_THREAD_CPUTIME)
printval("_POSIX_THREAD_CPUTIME", _POSIX_THREAD_CPUTIME);
#else
printval("_POSIX_THREAD_CPUTIME");
#endif
#if defined(_POSIX_THREAD_PRIO_INHERIT)
printval("_POSIX_THREAD_PRIO_INHERIT", _POSIX_THREAD_PRIO_INHERIT);
#else
printval("_POSIX_THREAD_PRIO_INHERIT");
#endif
#if defined(_POSIX_THREAD_PRIO_PROTECT)
printval("_POSIX_THREAD_PRIO_PROTECT", _POSIX_THREAD_PRIO_PROTECT);
#else
printval("_POSIX_THREAD_PRIO_PROTECT");
#endif
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
printval("_POSIX_THREAD_PRIORITY_SCHEDULING", _POSIX_THREAD_PRIORITY_SCHEDULING);
#else
printval("_POSIX_THREAD_PRIORITY_SCHEDULING");
#endif
#if defined(_POSIX_THREAD_PROCESS_SHARED)
printval("_POSIX_THREAD_PROCESS_SHARED", _POSIX_THREAD_PROCESS_SHARED);
#else
printval("_POSIX_THREAD_PROCESS_SHARED");
#endif
#if defined(_POSIX_THREAD_ROBUST_PRIO_INHERIT)
printval("_POSIX_THREAD_ROBUST_PRIO_INHERIT", _POSIX_THREAD_ROBUST_PRIO_INHERIT);
#else
printval("_POSIX_THREAD_ROBUST_PRIO_INHERIT");
#endif
#if defined(_POSIX_THREAD_ROBUST_PRIO_PROTECT)
printval("_POSIX_THREAD_ROBUST_PRIO_PROTECT", _POSIX_THREAD_ROBUST_PRIO_PROTECT);
#else
printval("_POSIX_THREAD_ROBUST_PRIO_PROTECT");
#endif
#if defined(_POSIX_THREADS)
printval("_POSIX_THREADS", _POSIX_THREADS);
#else
printval("_POSIX_THREADS");
#endif
#if defined(_POSIX_THREAD_SAFE_FUNCTIONS)
printval("_POSIX_THREAD_SAFE_FUNCTIONS", _POSIX_THREAD_SAFE_FUNCTIONS);
#else
printval("_POSIX_THREAD_SAFE_FUNCTIONS");
#endif
#if defined(_POSIX_THREAD_SPORADIC_SERVER)
printval("_POSIX_THREAD_SPORADIC_SERVER", _POSIX_THREAD_SPORADIC_SERVER);
#else
printval("_POSIX_THREAD_SPORADIC_SERVER");
#endif
#if defined(_POSIX_TIMEOUTS)
printval("_POSIX_TIMEOUTS", _POSIX_TIMEOUTS);
#else
printval("_POSIX_TIMEOUTS");
#endif
#if defined(_POSIX_TIMERS)
printval("_POSIX_TIMERS", _POSIX_TIMERS);
#else
printval("_POSIX_TIMERS");
#endif
#if defined(_POSIX_TIMESTAMP_RESOLUTION)
printval("_POSIX_TIMESTAMP_RESOLUTION", _POSIX_TIMESTAMP_RESOLUTION);
#else
printval("_POSIX_TIMESTAMP_RESOLUTION");
#endif
#if defined(_POSIX_TRACE)
printval("_POSIX_TRACE", _POSIX_TRACE);
#else
printval("_POSIX_TRACE");
#endif
#if defined(_POSIX_TRACE_EVENT_FILTER)
printval("_POSIX_TRACE_EVENT_FILTER", _POSIX_TRACE_EVENT_FILTER);
#else
printval("_POSIX_TRACE_EVENT_FILTER");
#endif
#if defined(_POSIX_TRACE_INHERIT)
printval("_POSIX_TRACE_INHERIT", _POSIX_TRACE_INHERIT);
#else
printval("_POSIX_TRACE_INHERIT");
#endif
#if defined(_POSIX_TRACE_LOG)
printval("_POSIX_TRACE_LOG", _POSIX_TRACE_LOG);
#else
printval("_POSIX_TRACE_LOG");
#endif
#if defined(_POSIX_TYPED_MEMORY_OBJECTS)
printval("_POSIX_TYPED_MEMORY_OBJECTS", _POSIX_TYPED_MEMORY_OBJECTS);
#else
printval("_POSIX_TYPED_MEMORY_OBJECTS");
#endif
#if defined(_POSIX_V6_ILP32_OFF32)
printval("_POSIX_V6_ILP32_OFF32", _POSIX_V6_ILP32_OFF32);
#else
printval("_POSIX_V6_ILP32_OFF32");
#endif
#if defined(_POSIX_V6_ILP32_OFFBIG)
printval("_POSIX_V6_ILP32_OFFBIG", _POSIX_V6_ILP32_OFFBIG);
#else
printval("_POSIX_V6_ILP32_OFFBIG");
#endif
#if defined(_POSIX_V6_LP64_OFF64)
printval("_POSIX_V6_LP64_OFF64", _POSIX_V6_LP64_OFF64);
#else
printval("_POSIX_V6_LP64_OFF64");
#endif
#if defined(_POSIX_V6_LPBIG_OFFBIG)
printval("_POSIX_V6_LPBIG_OFFBIG", _POSIX_V6_LPBIG_OFFBIG);
#else
printval("_POSIX_V6_LPBIG_OFFBIG");
#endif
#if defined(_POSIX_V7_ILP32_OFF32)
printval("_POSIX_V7_ILP32_OFF32", _POSIX_V7_ILP32_OFF32);
#else
printval("_POSIX_V7_ILP32_OFF32");
#endif
#if defined(_POSIX_V7_ILP32_OFFBIG)
printval("_POSIX_V7_ILP32_OFFBIG", _POSIX_V7_ILP32_OFFBIG);
#else
printval("_POSIX_V7_ILP32_OFFBIG");
#endif
#if defined(_POSIX_V7_LP64_OFF64)
printval("_POSIX_V7_LP64_OFF64", _POSIX_V7_LP64_OFF64);
#else
printval("_POSIX_V7_LP64_OFF64");
#endif
#if defined(_POSIX_V7_LPBIG_OFFBIG)
printval("_POSIX_V7_LPBIG_OFFBIG", _POSIX_V7_LPBIG_OFFBIG);
#else
printval("_POSIX_V7_LPBIG_OFFBIG");
#endif
#if defined(_POSIX_VDISABLE)
printval("_POSIX_VDISABLE", _POSIX_VDISABLE);
#else
printval("_POSIX_VDISABLE");
#endif
#if defined(_XOPEN_CRYPT)
printval("_XOPEN_CRYPT", _XOPEN_CRYPT);
#else
printval("_XOPEN_CRYPT");
#endif
#if defined(_XOPEN_ENH_I18N)
printval("_XOPEN_ENH_I18N", _XOPEN_ENH_I18N);
#else
printval("_XOPEN_ENH_I18N");
#endif
#if defined(_XOPEN_REALTIME)
printval("_XOPEN_REALTIME", _XOPEN_REALTIME);
#else
printval("_XOPEN_REALTIME");
#endif
#if defined(_XOPEN_REALTIME_THREADS)
printval("_XOPEN_REALTIME_THREADS", _XOPEN_REALTIME_THREADS);
#else
printval("_XOPEN_REALTIME_THREADS");
#endif
#if defined(_XOPEN_SHM)
printval("_XOPEN_SHM", _XOPEN_SHM);
#else
printval("_XOPEN_SHM");
#endif
#if defined(_XOPEN_STREAMS)
printval("_XOPEN_STREAMS", _XOPEN_STREAMS);
#else
printval("_XOPEN_STREAMS");
#endif
#if defined(_XOPEN_UNIX)
printval("_XOPEN_UNIX", _XOPEN_UNIX);
#else
printval("_XOPEN_UNIX");
#endif
#if defined(_XOPEN_UUCP)
printval("_XOPEN_UUCP", _XOPEN_UUCP);
#else
printval("_XOPEN_UUCP");
#endif

summary();

#if defined(_POSIX_C_SOURCE)
printval("_POSIX_C_SOURCE", _POSIX_C_SOURCE);
#else
printval("_POSIX_C_SOURCE");
#endif
#if defined(_POSIX_SOURCE)
printval("_POSIX_SOURCE", _POSIX_SOURCE);
#else
printval("_POSIX_SOURCE");
#endif
#if defined(_XOPEN_SOURCE)
printval("_XOPEN_SOURCE", _XOPEN_SOURCE);
#else
printval("_XOPEN_SOURCE");
#endif
#if defined(_XOPEN_SOURCE_EXTENDED)
printval("_XOPEN_SOURCE_EXTENDED", _XOPEN_SOURCE_EXTENDED);
#else
printval("_XOPEN_SOURCE_EXTENDED");
#endif
#if defined(_POSIX_VERSION)
printval("_POSIX_VERSION", _POSIX_VERSION);
#else
printval("_POSIX_VERSION");
#endif
#if defined(_POSIX2_VERSION)
printval("_POSIX2_VERSION", _POSIX2_VERSION);
#else
printval("_POSIX2_VERSION");
#endif
#if defined(_XOPEN_VERSION)
printval("_XOPEN_VERSION", _XOPEN_VERSION);
#else
printval("_XOPEN_VERSION");
#endif
}

int main() {
    test_posix();
    return 0;
}
