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

void buttonPC13_init() {
/* Configure PC13 as inputs, no pull */
	GPIOC.MODER = GPIOC.MODER & ~(0xFF<<6 | 3<<26);
	GPIOC.PUPDR = GPIOC.PUPDR & ~(0xFF<<6 | 3<<26);
}

void buttonPC13_irq_init() {
  /* set PC13 as EXTI13 input */
  SYSCFG.EXTICR4 = (SYSCFG.EXTICR4 & ~(0xF0)) | (0x20);

  /* Setup interrupt for EXTI13, falling edge */
  EXTI.IMR  |= (1<<13);
  EXTI.RTSR &= ~(1<<13);
  EXTI.FTSR |= (1<<13);

  /* enable EXTI15_10 IRQ */
  NVIC.ISER[40/32]=(1<<(40%32)); 
}

uint32_t buttonPC13_poll() {
  return (~(GPIOC.IDR>>13) & 1); /* 1 if pressed */
}

uint32_t button_poll() {
  return (~(GPIOB.IDR>>8) & 1); /* 1 if pressed */
}


