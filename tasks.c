

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "nrf24l01p.h"
#include "kernel.h"
#include "tasks.h"

void Tasks_Disable_Peripherals(void){
  //ADC and Analog comparator will be 
  //turned off automatically by the kernel
  //Put radio to deep sleep
  
}

void Task_RGB_LED(void){

  //Red, Green, Blue
  DDRD |= (1<<5)|(1<<6)|(1<<7);
  //Turn off all LEDs
  PORTD|= (1<<5)|(1<<6)|(1<<7);
  
  while(1){

    //Red LED on
    PORTD &=~ (1<<5);

    //Blocking delay
    _delay_us(100);

    //Red LED off
    PORTD |=  (1<<5);

    //Delay 5000 ms
    Kernel_Task_Sleep(5000/KER_TICK_TIME);

  }
}

void Task_Vin_Sense(void){
  
  //Init VinSense
  DDRD  |= (1<<2);

  //Disable VinSense
  PORTD &=~(1<<2);

  while(1){

    //Vin Sample
    Peripherals_Vin_Sense();
    
    //Delay 5000ms
    Kernel_Task_Sleep(5000/KER_TICK_TIME);
	
  }
}

void Task_Radio(void){
  
  uint16_t temp;
  uint8_t  buf[32];

  //Radio init with deep sleep
  nRF24L01P_Init();

  while(1){
    
    temp = Peripherals_Vin_RawADC();
    buf[0] = ((temp/1000) & 0xFF) + 48;
    buf[1] = ((temp/100) & 0xFF) + 48;
    buf[2] = ((temp/10) & 0xFF) + 48;
    buf[3] = ((temp/1) & 0xFF) + 48;
    nRF24L01P_WakeUp();
    //nRF24L01P_Transmit_Basic(buf, 4);
    nRF24L01P_Deep_Sleep();
    Kernel_Task_Sleep(5000/KER_TICK_TIME);
	
  }
}

void Task_Sensor(void){
  
  //Init and Disable SenseEn
  DDRD  |= (1<<3);
  PORTD |= (1<<3);
  
  while(1){

    
    Kernel_Task_Sleep(5000/KER_TICK_TIME);
	
  }
}
