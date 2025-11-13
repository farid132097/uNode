

#ifndef  _TASKS_H_
#define  _TASKS_H_


#ifndef __ASSEMBLER__    //Only accessible via C

void     Tasks_Disable_Peripherals(void);
void     Task_RGB_LED(void);
void     Task_Vin_Sense(void);
void     Task_Radio(void);
void     Task_Sensor(void);
void     Tasks_Task4(void);
void     Tasks_Task5(void);
void     Tasks_Task6(void);
void     Tasks_Task7(void);
void     Tasks_Task8(void);
void     Tasks_Task9(void);

#endif

#endif