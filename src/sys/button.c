#include "sys/devices.h"
#include "sys/cm4.h"
#include "button.h"

void button_init() {
/* Configure PB3-PB6, PB8 as inputs, no pull */
	GPIOB.MODER = GPIOB.MODER & ~(0xFF<<6 | 3<<16);
	GPIOB.PUPDR = GPIOB.PUPDR & ~(0xFF<<6 | 3<<16);
}

void button_irq_init () {
  /* set PB8 as EXTI8 input */
  SYSCFG.EXTICR3 = (SYSCFG.EXTICR3 & ~(0xf<<0)) | (0x1<<0);

  /* Setup interrupt for EXTI8, falling edge */
  EXTI.IMR  |= (1<<8);
  EXTI.RTSR &= ~(1<<8);
  EXTI.FTSR |= (1<<8);

  /* enable EXTI15_10 IRQ */
  NVIC.ISER[23/32]=(1<<(23%32)); 
}

uint32_t button_poll() {
  return (~(GPIOB.IDR>>8) & 1); /* 1 if pressed */
}


