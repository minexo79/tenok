#include <stdio.h>
#include <time.h>

#include "shell.h"

void shell_cmd_uptime(int argc, char *argv[])
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    // TODO: printf() should support %lld
    int time_sec = (int) tp.tv_sec;

    printf("%d seconds up\n", time_sec);
}

HOOK_SHELL_CMD(uptime);
