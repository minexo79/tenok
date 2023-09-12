#ifndef __SIGNAL_H__
#define __SIGNAL_H__

/* supported signals by tenok */
#define SIGUSR1  10
#define SIGUSR2  12
#define SIGALRM  14
#define SIGPOLL  29
#define SIGSTOP  19
#define SIGTSTP  20
#define SIGCONT  18
#define SIGINT   2
#define SIGKILL  9

#define SIGNAL_CNT 9

typedef	int sigset_t;

union sigval {
    int   sival_int;
    void *sival_ptr;
};

typedef struct {
    int      si_signo;
    int      si_code;
    union sigval si_value;
} siginfo_t;

struct sigaction {
    union {
        void (*sa_handler)(int);
        void (*sa_sigaction)(int, siginfo_t *, void *);
    };
    sigset_t sa_mask;
    int      sa_flags;
};

int sigaction(int signum, const struct sigaction *act,
              struct sigaction *oldact);

#endif
