#include <stddef.h>

#include <kernel/kernel.h>

#include "bsp_drv.h"

int main(void)
{
    /* user-level driver initialization */
    led_init();
    bsp_driver_init();

    /* start the os */
    sched_start();

    return 0;
}
