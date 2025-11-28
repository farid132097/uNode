

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "sensors.h"

typedef struct peripherals_t{
  uint8_t  Status;
  uint16_t VinRawADC;
  uint32_t VinSense;
  uint16_t V3V3RawADC;
  uint32_t V3V3Sense;
  uint8_t  Error;
  uint8_t  StickyError;
}peripherals_t;


peripherals_t Peripherals ={
  .Status     = 0,
  .VinRawADC  = 0,
  .VinSense   = 0,
  .V3V3RawADC = 0,
  .V3V3Sense  = 0,
  .Error      = 0,
  .StickyError= 0
};


void Peripherals_ADC_Init(void){
  uint8_t tout = 0;
  if( !(ADCSRA & (1<<ADEN)) ){
    ADMUX   = (1<<REFS1)|(1<<REFS0)|0x0F;
    ADCSRA  = (1<<ADPS1)|(1<<ADPS2);
    ADCSRA |= (1<<ADEN) |(1<<ADSC);
    while (!(ADCSRA & (1<<ADIF))) {
      //add timeout management
      tout++;
      _delay_us(10);
      if(tout > 200){
        Peripherals.Error = 0x01;
        break;
      }

    }
  }
}

uint16_t Peripherals_ADC_Sample(uint8_t channel, uint8_t nsamples){
  uint8_t  temp, tout;
  uint32_t val = 0;
  Peripherals_ADC_Init();
  if(Peripherals.Error == 0){
    temp  = ADMUX;
    temp &= 0xF0;
    temp |= channel;
    ADMUX = temp;
    _delay_us(500);
    for(uint8_t i=0; i<nsamples; i++){
      tout = 0;
      ADCSRA |= (1<<ADSC);
      while (!(ADCSRA & (1<<ADIF))) {
        //add timeout management
        tout++;
        _delay_us(10);
        if(tout > 200){
          Peripherals.Error = 0x01;
          break;
        }
      }
      val += ADCW;
    }
    val /= nsamples;
  }
  else{
    Peripherals.StickyError = Peripherals.Error;
    Peripherals.Error = 0;
    Peripherals_ADC_Init();
  }
  return (uint16_t)val;
}

void Peripherals_Vin_Sense_Sample(void){
  //Status bit 0 indicates Vinsense is active
  Peripherals.Status |= (1<<0);
  //Enable VinSense
  PORTD |= (1<<2);
  //Sample ADC
  Peripherals.VinRawADC = Peripherals_ADC_Sample(6, 4);
  //Disable VinSense
  PORTD &=~(1<<2);
  //Reference in mV
  Peripherals.VinSense  = 1100;
  //Voltage divider factor
  Peripherals.VinSense *= 21;
  Peripherals.VinSense *= Peripherals.VinRawADC;
  Peripherals.VinSense >>= 10;
  Peripherals.Status &=~(1<<0);
}

uint16_t Peripherals_Vin_RawADC_Get(void){
  return Peripherals.VinRawADC;
}

uint16_t Peripherals_Vin_Get(void){
  return (uint16_t)Peripherals.VinSense;
}

int16_t Peripherals_Analog_Temp_Get(void){
  return 0;
}

int16_t Peripherals_Digital_Temp_Get(void){
  return Sensors_HDC1080_Temp_Get();
}

uint16_t Peripherals_Digital_RH_Get(void){
  return Sensors_HDC1080_RH_Get();
}