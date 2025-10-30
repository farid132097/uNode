

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "kernel.h"
#include "tasks.h"

void Tasks_Disable_Peripherals(void)
{
    
}

void Task_RGB_LED(void)
{
    //Red, Blue, Green
    DDRD |= (1<<5)|(1<<6)|(1<<7);
    PORTD|= (1<<5)|(1<<6)|(1<<7);
  
    while(1)
    {
        Kernel_Task_Sleep(30000/KER_TICK_TIME);
    }
}

void Task_Vin_Sense(void){
  
  //Init and Disable Vinsense
  DDRD  |= (1<<2);
  PORTD &=~(1<<2);

  while(1){
    
    Peripherals_ADC_Sample(6, 4);
    Kernel_Task_Sleep(60000/KER_TICK_TIME);
	
  }
}

void Tasks_Task3(void){
  

  
  while(1){

    
    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	
  }
}

void Tasks_Task4(void){

  
  while(1){

    
    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	
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