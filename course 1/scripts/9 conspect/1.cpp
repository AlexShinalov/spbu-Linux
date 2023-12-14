#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>

using namespace std;

int main() {
	pid_t id1 = getpid(), id2 = syscall(SYS_gettid);
	cout << id1 << ' ' << id2 << '\n';
	return 0;
}
