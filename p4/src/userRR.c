#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "pstat.h"

int parseInt(char *str) {
    int number = 0;
    for (int ps = 0; str[ps] != '\0'; ps++) {
        int thisnum = str[ps] - '0';
        number = number * 10 + thisnum;
    }
    return number;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf(1, "Usage: userRR <user-level-timeslice> <iterations> <job> <jobcount>\n");
        exit();
    }
    int timeslice = parseInt(argv[1]);
    int iterations = parseInt(argv[2]);
    char* job = argv[3];
    int job_count = parseInt(argv[4]);
    int pids[job_count];
    for (int i = 0; i < job_count; i++) {
        pids[i] = fork2(1);
        if (pids[i] < 0) {
        } else if (pids[i] == 0) {
            // child process
            char* temp_argv[] = {job, 0};
            exec(job, temp_argv);
            printf(1, "exec failed\n");
            exit();
        }
    }
    for (int itr = 0; itr < iterations; itr++) {
        for(int i = 0; i < sizeof(pids)/sizeof(pids[0]); i++) {
            if (pids[i] == -1) {
                continue;
            }
            if (setpri(pids[i], 2) < 0) {
                printf(1, "setpri failed\n");
            }
            sleep(timeslice);
            setpri(pids[i], 1);
        }
    }
    struct pstat outStat = {0};
    getpinfo(&outStat);
    static char *states[] = {
    [UNUSED]    "unused",
    [EMBRYO]    "embryo",
    [SLEEPING]  "sleep ",
    [RUNNABLE]  "runble",
    [RUNNING]   "run   ",
    [ZOMBIE]    "zombie"
    };
    for (int i = 0; i < NPROC; i++) {
        char *state;
        if (outStat.state[i] >= 0) {
            state = states[outStat.state[i]];
        }
        printf(1, "[%d] inuse:%d priority:%d state:%s ticks: ",
                                outStat.pid[i],
                                outStat.inuse[i],
                                outStat.priority[i],
                                state);
        for (int j = NLAYER-1; j >= 0; j--) {
            printf(1, "%d ", outStat.ticks[i][j]);
        }
        printf(1, "qtail: ");
        for (int j = NLAYER-1; j >= 0; j--) {
            printf(1, "%d ", outStat.qtail[i][j]);
        }
        printf(1, "\n");
    }
    for(int i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
      if(pids[i] == -1)
        continue;
      kill(pids[i]);
      wait();
    }
    exit();

}
