#include <stdint.h>

#include <arch/port.h>
#include <kernel/kernel.h>

#include "stm32f4xx.h"

#define IRQ_START 16
#define IRQ_CNT 98

#define ISR __attribute__((weak, alias("Default_Handler")))

void Default_Handler(void);
void irq_handler(void);

void Reset_Handler(void);
ISR void NMI_Handler(void);
ISR void HardFault_Handler(void);
ISR void MemManage_Handler(void);
ISR void BusFault_Handler(void);
ISR void UsageFault_Handler(void);
ISR void SVC_Handler(void);
ISR void DebugMon_Handler(void);
ISR void PendSV_Handler(void);
void SysTick_Handler(void);

extern uint32_t _estack;

typedef void (*irq_handler_t)(void);

__attribute__((section(".isr_vector")))
irq_handler_t isr_vectors[IRQ_CNT] = {
    (irq_handler_t)&_estack,
    /* exceptions */
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    /* interrupts */
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler,
    irq_handler
};

irq_handler_t irq_table[IRQ_CNT];

void Default_Handler(void)
{
    while(1);
}

void SysTick_Handler(void)
{
    /* set nvic pendsv bit */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void irq_handler(void)
{
    int irq = get_proc_mode();
    irq_table[irq]();
}

void irq_init(void)
{
    for(int i = IRQ_START; i < IRQ_CNT; i++) {
        irq_table[IRQ_START + i] = Default_Handler;
    }

    /* set interrupt priorities */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_SetPriority(SysTick_IRQn, KERNEL_INT_PRI << 4); //set PRI[7:4] = KERNEL_INT_PRI
    NVIC_SetPriority(SVCall_IRQn, KERNEL_INT_PRI << 4);  //set PRI[7:4] = KERNEL_INT_PRI

    /* enable the systick timer */
    SysTick_Config(SystemCoreClock / OS_TICK_FREQ);
}

int request_irq(unsigned int irq, irq_handler_t handler,
                unsigned long flags, const char *name,
                void *dev)
{
    irq_table[IRQ_START + irq] = handler;
}
