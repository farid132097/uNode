

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "nrf24l01p.h"
#include "kernel.h"
#include "tasks.h"

void Tasks_Disable_Peripherals(void){
  ADCSRA &=~(1<<ADEN);
  ACSR   |= (1<<ACD);
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

    //Enable VinSense
    PORTD |= (1<<2);

    //Vin Sample
    Peripherals_ADC_Init();

    //Enable VinSense
    PORTD &=~(1<<2);
    
    //Delay 6000ms
    Kernel_Task_Sleep(5000/KER_TICK_TIME);
	
  }
}

void Task_Radio(void){
  
  //Radio init
  nRF24L01P_Init();

  //SPI disable
  nRF24L01P_Disable_SPI();

  //Disable gpio for deep sleep
	nRF24L01P_Disable_GPIO();
  
  while(1){

    
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
