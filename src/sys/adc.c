#include "sys/devices.h"
#include "sys/cm4.h"
#include "sys/clock.h"

void ADC_set_prediv(uint32_t val) {
  ADC_common.CCR= (ADC_common.CCR & ~(3<<16)) | (val & 3)<<16;
}

void enable_Vrefint() {
  ADC_common.CCR|=(1<<23);
}
void enable_Vbat() {
  ADC_common.CCR|=(1<<22);
}
void disable_Vrefint() {
  ADC_common.CCR&=~(1<<23);
}
void disable_Vbat() {
  ADC_common.CCR&=~(1<<22);
}

float get_sampling_time_us(volatile struct ADCx_registers *ADCx, uint32_t channel) {
  static const uint32_t ADC_smpr_cycles[8] =
    {3,15,28,56,84,112,144,480};
  static const uint32_t ADC_prediv[4] =
    {2,4,6,8};
  uint32_t ADC_freq=get_APB2CLK()/ADC_prediv[(ADC_common.CCR>>16)&0x3];
  uint32_t ADC_cycles;
  if (channel<10) {
    uint32_t bit_offset=channel*3;
    uint32_t smpr=((ADCx->SMPR2)>>bit_offset)&0x7;
    ADC_cycles=ADC_smpr_cycles[smpr];
  } else if (channel<=18) {
    uint32_t bit_offset=(channel-10)*3;
    uint32_t smpr=((ADCx->SMPR1)>>bit_offset)&0x7;
    ADC_cycles=ADC_smpr_cycles[smpr];
  } else {
    ADC_cycles=0;
  }
  return (ADC_cycles/(ADC_freq/1000000.));
}

void ADC_set_sampling_time(volatile struct ADCx_registers *ADCx,
				   uint32_t channel,uint32_t smpr)
{
  if (channel <= 9)
    {    ADCx->SMPR2=
	(ADCx->SMPR2 & ~(7<<(3*channel))) |
	(smpr<<(3*channel));
    }
  else if (channel <= 18 )
    {
      ADCx->SMPR1=
	(ADCx->SMPR1 & ~(7<<(3*(channel-10)))) |
	(smpr<<(3*(channel-10)));
    }
}

void ADC_set_sequence(volatile struct ADCx_registers *ADCx,uint32_t* sequence,uint32_t len) {
  uint32_t i,sqr[3]={0,0,0};
  for (i=0;i<len;i++) {
    sqr[i/6]|=(sequence[i]&31)<<(5*(i%6));
  }
  sqr[2]|=(((len-1)&15)<<20);
  ADCx->SQR1=sqr[2];
  ADCx->SQR2=sqr[1];
  ADCx->SQR3=sqr[0];
}


void ADC_init_sequence(volatile struct ADCx_registers *ADCx,uint32_t* sequence,uint32_t len) {
  ADCx->CR2= 0x00000001; /* EOCS=1, ADON=1 */
  ADCx->CR1= 0x00000100; /* SCAN=1 */
  ADC_set_sequence(ADCx,sequence,len);
}

void ADC_init_single_channel(volatile struct ADCx_registers *ADCx,uint32_t channel) {
  ADCx->CR2= 0x00000001; /* EOCS=0, ADON=1 */
  ADCx->CR1= 0x00000000; /* SCAN=0 */
  ADC_set_sequence(ADCx,&channel,1);
}

uint32_t ADC_convert_sequence(volatile struct ADCx_registers *ADCx,uint32_t *data_buffer) {
  uint32_t len=((ADCx->SQR1>>20)&15) +1;
  uint32_t i=0;
  ADCx->SR = 0; /* clear flags */
  ADCx->CR2|=(1<<10); /* enable EOC after each conversion */
  ADCx->CR2|=(1<<30); /* SWSTART=1 */
  while (i<len) {
    while ((ADCx->SR&0x22)==0); /* Wait for EOC==1 or OVR==1 */
    if ((ADCx->SR&0x20)!=0) break;
    data_buffer[i++]=ADCx->DR; /* reading clears flag */
  }
  return i; /* i<len if overrun */
}

uint32_t ADC_convert_single_channel(volatile struct ADCx_registers *ADCx) {
  ADCx->SR = 0; /* clear flags */
  ADCx->CR2|=(1<<30); /* SWSTART=1 */
  while ((ADCx->SR&0x2)==0); /* Wait for EOC==1 */
  return ADCx->DR;
}
