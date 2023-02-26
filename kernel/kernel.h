#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>
#include <stdbool.h>
#include "kconfig.h"
#include "list.h"

#define	PRIO_PROCESS 0
#define	PRIO_PGRP    1
#define	PRIO_USER    2

#define DEF_SYSCALL(func, _num) \
        {.syscall_handler = sys_ ## func, .num = _num}

#define DEF_IRQ_ENTRY(func, _num) \
        {.syscall_handler = func, .num = _num}

typedef struct {
    void (* syscall_handler)(void);
    uint32_t num;
} syscall_info_t;

typedef void (*task_func_t)(void *);

enum {
    TASK_WAIT,
    TASK_READY,
    TASK_RUNNING
} TASK_STATUS;

/* layout of the user stack */
struct task_stack {
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t _lr;
    uint32_t _r7; //syscall number
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
    uint32_t stack[TASK_STACK_SIZE - 17];
};

/* task control block */
struct task_ctrl_blk {
    struct task_stack *stack_top;    //pointer of the stack top address
    uint32_t stack[TASK_STACK_SIZE]; //stack memory
    uint32_t stack_size;

    uint8_t  status;
    uint32_t pid;
    int      priority;
    char     name[TASK_NAME_LEN_MAX];

    uint32_t remained_ticks;

    bool     syscall_pending;

    struct {
        size_t size;
    } file_request;

    struct list list;
};

void os_service_init(void);
void sched_start(task_func_t first_task);

void os_env_init(uint32_t stack);
uint32_t *jump_to_user_space(uint32_t stack);

void prepare_to_wait(struct list *q, struct list *wait, int state);
void wake_up(struct list *wait_list);

void preempt_disable(void);
void preempt_enable(void);

void reset_basepri(void);
void set_basepri(void);

void sprint_tasks(char *str, size_t size);

#endif
