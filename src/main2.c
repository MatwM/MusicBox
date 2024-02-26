#include <stdio.h>
#include <math.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"




int main() {
  
  printf("\e[2J\e[1;1H\r\n*** Welcome to Nucleo F446 ! ***\r\n");
  printf("   %08lx-%08lx-%08lx\r\n",U_ID[0],U_ID[1],U_ID[2]);
  printf("SYSCLK = %9lu Hz\r\n",get_SYSCLK());
  printf("AHBCLK = %9lu Hz\r\n",get_AHBCLK());
  printf("APB1CLK= %9lu Hz\r\n",get_APB1CLK());
  printf("APB2CLK= %9lu Hz\r\n",get_APB2CLK());
  printf("\r\n");
  
return 0;
}

