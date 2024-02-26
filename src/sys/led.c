#include <stdio.h>
#include <math.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"
#include "sys/led.h"


void led_init() {
	GPIOA.MODER = (GPIOA.MODER & ~(0x3FFF<<8)) | (0x1555<<8);
	GPIOA.OTYPER  &= ~(0x7F<<4);
	GPIOA.OSPEEDR |= (0x3FFF<<8);
}
