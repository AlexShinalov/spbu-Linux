#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <string>

using namespace std;

const int MAX_SIZE = 100;
string name;
char name3[MAX_SIZE], name2[MAX_SIZE];

int changePCName(void *ptr) {
	sethostname(name.data(), name.length());
	gethostname(name2, MAX_SIZE);
	clog << "Child hostname: " << name2 << '\n';
	return 0;
}

int main(int n, char *argv[]) {
	
	name = argv[1];
	gethostname(name3, MAX_SIZE);
	clog << "Parent hostname: " << name3 << '\n';

	const int STACK_SIZE = 1 << 14;
	void *stack = malloc(STACK_SIZE);

	auto pid = clone(changePCName, (char*)stack + STACK_SIZE, CLONE_NEWUSER | CLONE_NEWUTS | SIGCHLD, 0);

	int stat = 0;
	wait(&stat);

	cout << "Child process: " << pid;
	return 0;
}

