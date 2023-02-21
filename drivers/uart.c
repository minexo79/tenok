#include "string.h"
#include "stm32f4xx.h"
#include "semaphore.h"
#include "uart.h"
#include "fs.h"
#include "mqueue.h"
#include "syscall.h"

#define ENABLE_UART3_DMA 0 //QEMU does not support dma emulation for stm32f4

static void uart3_dma_puts(const char *data, size_t size);

ssize_t serial0_read(struct file *filp, char *buf, size_t size, loff_t offset);
ssize_t serial0_write(struct file *filp, const char *buf, size_t size, loff_t offset);

sem_t sem_uart3_tx;
mqd_t mq_uart3_rx;

static struct file_operations serial0_file_ops = {
	.read = serial0_read,
	.write = serial0_write
};

void serial0_init(void)
{
	/* initialize serial0 as character device */
	register_chrdev("serial0", &serial0_file_ops);

	/* initialize the message queue for reception */
	struct mq_attr attr = {
		.mq_flags = O_NONBLOCK,
		.mq_maxmsg = 100,
		.mq_msgsize = sizeof(uint8_t),
		.mq_curmsgs = 0
	};
	mq_uart3_rx = mq_open("/serial0_mq_rx", 0, &attr);

	/* initialize the semaphore for transmission */
	sem_init(&sem_uart3_tx, 0, 0);

	/* enable the uart interrupt service routine */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

ssize_t serial0_read(struct file *filp, char *buf, size_t size, loff_t offset)
{
	return mq_receive(mq_uart3_rx, (char *)buf, size, 0);
}

ssize_t serial0_write(struct file *filp, const char *buf, size_t size, loff_t offset)
{
#if (ENABLE_UART3_DMA != 0)
	uart3_dma_puts(buf, size);
#else
	uart_puts(USART3, buf, size);
#endif
	return size;
}

void uart3_init(uint32_t baudrate)
{
	/* initialize the rcc */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	/* initialize the gpio */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
	GPIO_InitTypeDef gpio = {
		.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP
	};
	GPIO_Init(GPIOC, &gpio);

	/* initialize the uart3 */
	USART_InitTypeDef uart3 = {
		.USART_BaudRate = baudrate,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No
	};
	USART_Init(USART3, &uart3);
	USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);

	/* enable uart3's interrupt */
	NVIC_InitTypeDef nvic = {
		.NVIC_IRQChannel = USART3_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 10,
		.NVIC_IRQChannelSubPriority = 0,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&nvic);

#if (ENABLE_UART3_DMA != 0)
	/* enable dma1's interrupt */
	nvic.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_Init(&nvic);
#endif
}

static void uart3_dma_puts(const char *data, size_t size)
{
	/* initialize the dma */
	DMA_InitTypeDef DMA_InitStructure = {
		.DMA_BufferSize = (uint32_t)size,
		.DMA_FIFOMode = DMA_FIFOMode_Disable,
		.DMA_FIFOThreshold = DMA_FIFOThreshold_Full,
		.DMA_MemoryBurst = DMA_MemoryBurst_Single,
		.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
		.DMA_MemoryInc = DMA_MemoryInc_Enable,
		.DMA_Mode = DMA_Mode_Normal,
		.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR),
		.DMA_PeripheralBurst = DMA_PeripheralBurst_Single,
		.DMA_PeripheralInc = DMA_PeripheralInc_Disable,
		.DMA_Priority = DMA_Priority_Medium,
		.DMA_Channel = DMA_Channel_7,
		.DMA_DIR = DMA_DIR_MemoryToPeripheral,
		.DMA_Memory0BaseAddr = (uint32_t)data
	};
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);

	/* enable dma and its interrupt */
	DMA_Cmd(DMA1_Stream4, ENABLE);
	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);

	/* trigger data sending */
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

	/* wait until the dma complete the transmission */
	sem_wait(&sem_uart3_tx);
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
		uint8_t c = USART_ReceiveData(USART3);
		mq_send(mq_uart3_rx, (char *)&c, 1, 0);
	}
}

void DMA1_Stream4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET) {
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF3);
		DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, DISABLE);
		sem_post(&sem_uart3_tx);
	}
}


void uart_putc(USART_TypeDef *uart, char c)
{
	while(USART_GetFlagStatus(uart, USART_FLAG_TXE) == RESET);
	USART_SendData(uart, c);
	while(USART_GetFlagStatus(uart, USART_FLAG_TC) == RESET);
}

char uart_getc(USART_TypeDef *uart)
{
	while(USART_GetFlagStatus(uart, USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData(uart);
}

void uart_puts(USART_TypeDef *uart, const char *data, size_t size)
{
	int i;
	for(i = 0; i < size; i++)
		uart_putc(uart, data[i]);
}
