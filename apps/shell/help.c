#include "shell.h"

void shell_cmd_help(int argc, char argv[SHELL_ARG_CNT][SHELL_ARG_LEN])
{
    char *s = "supported commands:\n\r"
              "help, clear, history, ps, echo, ls, cd, pwd, cat, file, mpool\n\r";
    shell_puts(s);
}
