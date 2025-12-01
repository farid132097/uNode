

#ifndef  _TASKS_H_
#define  _TASKS_H_


#ifndef __ASSEMBLER__    //Only accessible via C

void     Task_Disable_Peripherals(void);
void     Task_RGB_LED(void);
void     Task_Vin_Sense(void);
void     Task_Radio(void);
void     Task_Sensor(void);
void     Task_Print_Test(void);

#endif

#endif


