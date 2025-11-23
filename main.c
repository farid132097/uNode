

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "kernel.h"
#include "tasks.h"
#include "debug.h"



int main(void) {
    
	Kernel_Clock_Prescale(1);
	
	Debug_Init(0);
	Kernel_Init();
	Kernel_Task_Create(Task_RGB_LED,  0);
	Kernel_Task_Create(Task_Vin_Sense,  1);
	Kernel_Task_Create(Task_Radio,  2);
	Kernel_Task_Create(Task_Sensor,  3);
	Kernel_PreSleep_Hook(Tasks_Disable_Peripherals);
	Kernel_Start_Tasks();
	
    while(1){
		//Nothing to do here
		//All tasks are handled by kernel
    }
}

