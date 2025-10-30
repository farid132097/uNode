

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"

typedef struct peripherals_t
{
    uint8_t  Status;
    uint16_t VinRawADC;
    uint16_t VinSense;
    uint16_t V3V3RawADC;
    uint16_t V3V3Sense;
}peripherals_t;


peripherals_t Peripherals =
{
    .Status     = 0,
    .VinRawADC  = 0,
    .VinSense   = 0,
    .V3V3RawADC = 0,
    .V3V3Sense  = 0
};


void Peripherals_ADC_Init(void)
{
    if( !(ADCSRA & (1<<ADEN)) )
    {
        ADMUX   = (1<<REFS1)|(1<<REFS0)|0x0F;
        ADCSRA  = (1<<ADPS1)|(1<<ADPS2);
        ADCSRA |= (1<<ADEN) |(1<<ADSC);
        while (!(ADCSRA & (1<<ADIF))) 
        {
            //add timeout management
        }
    }
}

uint16_t Peripherals_ADC_Sample(uint8_t channel, uint8_t nsamples)
{
    uint8_t  temp;
    uint32_t val = 0;
    Peripherals_ADC_Init();
    temp  = ADMUX;
    temp &= 0xF0;
    temp |= channel;
    ADMUX = temp;
    for(uint8_t i=0; i<nsamples; i++)
    {
        ADCSRA |= (1<<ADSC);
        while (!(ADCSRA & (1<<ADIF))) 
        {
            //add timeout management
        }
        val += ADCW;
    }
    val /= nsamples;
    return (uint16_t)val;
}

void Peripherals_Vin_Sense(void)
{
    //Status bit 0 indicates Vinsense is active
    Peripherals.Status |= (1<<0);
    PORTD |= (1<<2);
    Peripherals.VinRawADC = Peripherals_ADC_Sample(6, 4);
    PORTD |= (1<<2);
    Peripherals.Status &=~(1<<0);
}