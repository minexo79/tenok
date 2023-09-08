#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "kernel.h"
#include "syscall.h"
#include "uart.h"
#include "mutex.h"
#include "task.h"

pthread_mutex_t mutex_print;

void mutex_task1(void)
{
    set_program_name("mutex1");

    pthread_mutex_init(&mutex_print, NULL);

    int serial_fd = open("/dev/serial0", 0, 0);
    char *str = "mutex task 1 is running\n\r";

    while(1) {
        pthread_mutex_lock(&mutex_print);
        write(serial_fd, str, strlen(str));
        pthread_mutex_unlock(&mutex_print);

        sleep(1000);
    }
}

void mutex_task2(void)
{
    set_program_name("mutex2");

    int serial_fd = open("/dev/serial0", 0, 0);
    char *str = "mutex task 2 is running\n\r";

    while(1) {
        pthread_mutex_lock(&mutex_print);
        write(serial_fd, str, strlen(str));
        pthread_mutex_unlock(&mutex_print);
        sleep(1000);
    }
}

HOOK_USER_TASK(mutex_task1);
HOOK_USER_TASK(mutex_task2);
