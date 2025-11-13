

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "nrf24l01p.h"
#include "kernel.h"
#include "tasks.h"

void Tasks_Disable_Peripherals(void)
{
    
}

void Task_RGB_LED(void)
{
    //Red, Green, Blue
    DDRD |= (1<<5)|(1<<6)|(1<<7);
    PORTD|= (1<<5)|(1<<6)|(1<<7);
  
    while(1)
    {
        PORTD &=~ (1<<6);
        _delay_ms(2);
        PORTD |=  (1<<6);
        Kernel_Task_Sleep(3000/KER_TICK_TIME);
    }
}

void Task_Vin_Sense(void){
  
  //Init and Disable Vinsense
  DDRD  |= (1<<2);
  PORTD &=~(1<<2);

  while(1){
    
    Peripherals_ADC_Sample(6, 4);
    Kernel_Task_Sleep(6000/KER_TICK_TIME);
	
  }
}

void Task_Radio(void){
  
  nRF24L01P_Init();
  
  while(1){

    
    Kernel_Task_Sleep(3000/KER_TICK_TIME);
	
  }
}

void Task_Sensor(void){
  
  //Init and Disable SenswEn
  DDRD  |= (1<<3);
  PORTD |= (1<<3);
  
  while(1){

    
    Kernel_Task_Sleep(3000/KER_TICK_TIME);
	
  }
}

void Tasks_Task5(void){
  
  while(1){
    
    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	  
  }
}

void Tasks_Task6(void){
  
  
  while(1){

    
    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	
  }
}

void Tasks_Task7(void){
  
  
  while(1){

    
    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	
  }
}

void Tasks_Task8(void){
  
  
  while(1){
    
    
    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	
  }
}

void Tasks_Task9(void){
  
  
  while(1){
    
    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	
  }
}