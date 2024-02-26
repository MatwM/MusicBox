#include "sys/devices.h"
#include "sys/clock.h"

void SysTick_init(uint32_t freq) {
  uint32_t period=get_SYSCLK()/freq;
  SysTick.LOAD=((period-1)&0x00ffffff);
  SysTick.VAL=0;
  SysTick.CTRL|=0x7;
}

void SysTick_deinit() {
  SysTick.CTRL &= ~0x7;
}
