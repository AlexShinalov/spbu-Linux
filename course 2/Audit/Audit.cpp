#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>
#include "constant.h"
#include <sstream>
#include <iomanip>

using namespace std;
string current_time() {
    time_t now = time(0);
    tm* time_struct = localtime(&now);

    ostringstream line_stream;
    line_stream << put_time(time_struct, "%d.%m.%Y %H:%M:$S");
    return line_stream.str();
}

void write_system(unsigned long long system_code, ofstream &file, int pid) {
    file << pid << ":";
    file << current_time() << ": ";
    file << system_names[system_code] << " code=" << system_code << endl;
}

void write_command(const char* line, ofstream &file, int pid) {
    file << pid << ":";
   file << current_time() << ": ";
        file << line << endl;
    }



int main(int argc, char** argv)
{
    setlogmask(LOG_UPTO(LOG_NOTICE));
    if (argc != 2)
    {
        syslog(LOG_INFO, "wrong arguments");
        return -1;
    }

    int pid = stoi(argv[1]); 
    ofstream file("logs.log", ios::app);

    write_command("ptrace is afixed", file, pid);
    ptrace(PTRACE_ATTACH, pid, nullptr, nullptr);
    if (errno == -1) {
        return errno;
    }

    write_command("ptrace set options", file, pid);
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
    if (errno == -1) {
        return errno;
    }
    int status;
    waitpid(pid, &status, 0);
    user_regs_struct regs;
    while (WIFSTOPPED(status)) {
        ptrace(PTRACE_SYSCALL, pid, nullptr, nullptr);
        if (errno == -1) return -1;
        waitpid(pid, &status, 0);
        ptrace(PTRACE_GETREGS, pid, 0, &regs);
        if (errno == -1) {
            return errno;
        }
        write_system(regs.orig_rax, file, pid);
    }

    ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
    write_command("ptrace listening system calls", file, pid);

    return 0;
}
