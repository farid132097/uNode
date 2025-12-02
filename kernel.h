
/*
 * File          : kernel.h
 * Author        : MD. Faridul Islam (faridmdislam@gmail.com)
 * Description   : AVR kernel for bare-metal RTOS
 * Created       : Jul 27, 2025, 9:30 PM
 * Last Modified : Dec 03, 2025, 01:18 AM
 */



#ifndef  _KERNEL_H_
#define  _KERNEL_H_



//Enable or disable kernel debug via gpio
//#define  KER_DBG_ENABLE

//Uncomment only one
//#define  KER_TIMER0_AS_TICK_SRC
//#define  KER_WDT_AS_TICK_SRC
#define  KER_TOSC_AS_TICK_SRC

//Uncomment only one
//#define  KER_WDT_TICK_16MS
//#define  KER_WDT_TICK_32MS
//#define  KER_WDT_TICK_64MS
#define  KER_WDT_TICK_125MS
//#define  KER_WDT_TICK_250MS
//#define  KER_WDT_TICK_500MS
//#define  KER_WDT_TICK_1000MS

//Uncomment only one
//#define  KER_TOSC_TICK_1MS
//#define  KER_TOSC_TICK_10MS
//#define  KER_TOSC_TICK_50MS
//#define  KER_TOSC_TICK_100MS
//#define  KER_TOSC_TICK_250MS
//#define  KER_TOSC_TICK_500MS
#define  KER_TOSC_TICK_1000MS



//In idle time, cpu will be in sleep
#define  KER_IDLE_AS_SLEEP

//Before entering sleep, several peripherals needs to be disabled
//User can hook a function which will disable peripherals before sleep
//Add function by calling Kernel_PreSleep_Hook(func_name) before Kernel_Start_Tasks() 
//#define  KER_CALL_FUNC_BEFORE_SLEEP


//Define IO address of the DDR & PORT for debug
#ifdef   KER_DBG_ENABLE
#define  KER_DBG_DDR     0x0A   //DDRD IO address
#define  KER_DBG_PORT    0x0B   //PORTD IO address
#define  KER_DBG_PIN     0x05   //PORTD5 pin
#endif



//Do not change below section
#ifdef KER_TIMER0_AS_TICK_SRC
#define  KER_SLEEP_MODE_IDLE
#define KER_TICK_TIME 1U
#endif

#ifdef KER_WDT_AS_TICK_SRC
#define  KER_SLEEP_MODE_POWER_DOWN
#ifdef KER_WDT_TICK_16MS
#define KER_TICK_TIME 16U
#endif
#ifdef KER_WDT_TICK_32MS
#define KER_TICK_TIME 32U
#endif
#ifdef KER_WDT_TICK_64MS
#define KER_TICK_TIME 64U
#endif
#ifdef KER_WDT_TICK_125MS
#define KER_TICK_TIME 125U
#endif
#ifdef KER_WDT_TICK_250MS
#define KER_TICK_TIME 250U
#endif
#ifdef KER_WDT_TICK_500MS
#define KER_TICK_TIME 500L
#endif
#ifdef KER_WDT_TICK_1000MS
#define KER_TICK_TIME 1000L
#endif
#endif

//Do not change below section
#ifdef KER_TOSC_AS_TICK_SRC
#define  KER_SLEEP_MODE_POWER_SAVE
#ifdef KER_TOSC_TICK_1MS
#define KER_TICK_TIME 1U
#endif
#ifdef KER_TOSC_TICK_10MS
#define KER_TICK_TIME 10U
#endif
#ifdef KER_TOSC_TICK_50MS
#define KER_TICK_TIME 50U
#endif
#ifdef KER_TOSC_TICK_100MS
#define KER_TICK_TIME 100U
#endif
#ifdef KER_TOSC_TICK_250MS
#define KER_TICK_TIME 250L
#endif
#ifdef KER_TOSC_TICK_500MS
#define KER_TICK_TIME 500L
#endif
#ifdef KER_TOSC_TICK_1000MS
#define KER_TICK_TIME 1000L
#endif
#endif





#ifndef __ASSEMBLER__           //Only accessible via C
extern void      Kernel_Tick_Init(uint8_t presclaer_reg, uint8_t reload_val);
extern void      Kernel_Task_Create(void (*func)(void), uint8_t priority);
extern void      Kernel_Start_Tasks(void);
extern void      Kernel_Init(void);
extern void      Kernel_Task_Idle(void);
extern void      Kernel_Task_Sleep(uint16_t val);
extern void      Kernel_Task_Constant_Latency(uint16_t val);
extern void      Kernel_Task_Constant_Latency_Sleep(void);
extern void      Kernel_PreSleep_Hook(void (*func)(void));
extern void      Kernel_Clock_Prescale(uint8_t prescaler_reg_val);
extern uint16_t  Kernel_Task_Sleep_Time_Get(uint8_t task_id);
extern uint8_t   Kernel_Task_Status_Get(uint8_t task_id);
extern uint8_t   Kernel_NTask_Get(void);
extern uint8_t   Kernel_Task_Prio_Get(uint8_t task_id);
extern uint8_t   Kernel_Lowest_Prio_Get(void);
extern uint8_t   Kernel_High_Prio_Task_ID_Get(void);
extern uint8_t   Kernel_Abs_High_Prio_Task_ID_Get(void);
extern uint8_t   Kernel_CPU_Usage_Get(void);
extern uint32_t  Kernel_Tick_Val_Get(void);
extern uint32_t  Kernel_Tick_Val_Safely_Get(void);

#endif



#endif


