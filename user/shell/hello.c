#include "shell.h"

int hello(int argc, char *argv[])
{
    shell_puts("Hello Tenok!");
    return 0;
}

HOOK_SHELL_CMD("hello", hello);
