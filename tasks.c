

#include <avr/io.h>
#include <util/delay.h>
#include "kernel.h"
#include "tasks.h"

void Tasks_Disable_Peripherals(void){
  
}

void Tasks_Task1(void){
  
  
  
  while(1){
    
    

    Kernel_Task_Sleep(5000/KER_TICK_TIME);
    
  }
}

void Tasks_Task2(void){

  
  while(1){
    

    

    Kernel_Task_Sleep(7000/KER_TICK_TIME);
	
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