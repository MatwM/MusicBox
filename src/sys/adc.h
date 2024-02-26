#include "sys/devices.h"

void ADC_set_prediv(uint32_t val); /* val = 0..3 */

void enable_Vrefint();
void disable_Vrefint();

void enable_Vbat();
void disable_Vbat();

float ADC_get_sampling_time_us(volatile struct ADCx_registers *ADCx, uint32_t channel);

void ADC_set_sampling_time(volatile struct ADCx_registers *ADCx, uint32_t channel,uint32_t smpr);


void ADC_init_single_channel(volatile struct ADCx_registers *ADCx,uint32_t channel);

uint32_t ADC_convert_single_channel(volatile struct ADCx_registers *ADCx);

void ADC_init_sequence(volatile struct ADCx_registers *ADCx,uint32_t* sequence,uint32_t len);

uint32_t ADC_convert_sequence(volatile struct ADCx_registers *ADCx,uint32_t *data_buffer);
