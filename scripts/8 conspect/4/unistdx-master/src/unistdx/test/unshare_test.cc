#define _GNU_SOURCE
#include <sched.h>
int main() {
    return unshare(CLONE_NEWUSER | CLONE_NEWUTS);
}
