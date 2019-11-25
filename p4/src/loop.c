#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int main(void) {
    for(;;) {
        sleep(10);
        printf(1, "%d\n", getpid());
    }
}
