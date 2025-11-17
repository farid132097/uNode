

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "kernel.h"
#include "tasks.h"



int main(void) 
{
    
	Kernel_Init();

	Kernel_Task_Create(Task_RGB_LED,  0);
	Kernel_Task_Create(Task_Vin_Sense,  1);
	Kernel_Task_Create(Task_Radio,  2);
	Kernel_Task_Create(Task_Sensor,  3);
	//Kernel_Task_Create(Tasks_Task5,  0);
	//Kernel_Task_Create(Tasks_Task6,  6);
	//Kernel_Task_Create(Tasks_Task7,  4);
	//Kernel_Task_Create(Tasks_Task8,  8);
	//Kernel_Task_Create(Tasks_Task9,  7);
	Kernel_PreSleep_Hook(Tasks_Disable_Peripherals);
	
	Kernel_Start_Tasks();
	
    while(1)
	{
		
        
    }
}

