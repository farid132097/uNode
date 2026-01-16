

#ifndef  _PERIPHERALS_H_
#define  _PERIPHERALS_H_

#define  PERIPHERAL_ADC_VREFINT_MV               1170
#define  PERIPHERAL_ADC_VIN_SCALING_FACTOR_X10   210
#define  PERIPHERAL_ADC_V3V3_SCALING_FACTOR_X10  110

void     Peripherals_ADC_Init(void);
uint16_t Peripherals_ADC_Sample(uint8_t channel, uint8_t nsamples);
void     Peripherals_Vin_Sense_Sample(void);
void     Peripherals_V3V3_Sense_Sample(void);
uint16_t Peripherals_Vin_RawADC_Get(void);
uint16_t Peripherals_Vin_Get(void);
uint16_t Peripherals_V3V3_RawADC_Get(void);
uint16_t Peripherals_V3V3_Get(void);
int16_t  Peripherals_Analog_Temp_Get(void);
int16_t  Peripherals_Digital_Temp_Get(void);
uint16_t Peripherals_Digital_RH_Get(void);

#endif


