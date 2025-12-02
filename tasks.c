

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "nrf24l01p.h"
#include "sensors.h"
#include "kernel.h"
#include "config.h"
#include "tasks.h"
#include "debug.h"
#include "rgb.h"

typedef struct task_data_t
{
  uint16_t Vin;
  int16_t  ATemp;
  int16_t  DTemp;
  uint16_t DRH;
  uint8_t  Buf[32];
  uint32_t uptime;
}task_data_t;

task_data_t TaskData;



void Task_Disable_Peripherals(void){
  //ADC and Analog comparator will be
  //turned off automatically by the kernel
}

__attribute__((noreturn)) void Task_RGB_LED(void){
  
  //Init RGB GPIOs
  RGB_Init();
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);
  
  
  while(1){
    
    //Red LED on
    if(Peripherals_Vin_Get() > 2800){
      RGB_Set_State(0,1,0);
    }
    else{
      RGB_Set_State(1,0,0);
    }
    
    
    //Blocking delay
    _delay_us(100);

    //Red LED off
    RGB_Set_State(0,0,0);

    //Delay 5000 ms
    Kernel_Task_Sleep(RGB_LED_TASK_SLEEP_DUR_MS/KER_TICK_TIME);

  }
}

__attribute__((noreturn)) void Task_Vin_Sense(void){
  
  //Init VinSense
  DDRD  |= (1<<2);
  //Disable VinSense
  PORTD &=~(1<<2);
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);
  
  while(1){
    
    //Vin Sample
    Peripherals_Vin_Sense_Sample();
    //Delay 5000ms
    Kernel_Task_Sleep(VIN_TASK_SLEEP_DUR_MS/KER_TICK_TIME);
	
  }
}

__attribute__((noreturn)) void Task_Radio(void){
  
  //Radio init with deep sleep
  nRF24L01P_Init();
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);

  #ifdef DEBUG_ENABLE_WITH_UBRR_VAL
  Debug_Init(DEBUG_ENABLE_WITH_UBRR_VAL);
  #endif

  while(1){
    
    //Process Node ID
    TaskData.Buf[0] = DEVICE_NODE_ID;
    
    //Process UpTime
    TaskData.uptime = Kernel_Tick_Val_Get();
    TaskData.Buf[1] = (TaskData.uptime >> 24) & 0xFF;
    TaskData.Buf[2] = (TaskData.uptime >> 16) & 0xFF;
    TaskData.Buf[3] = (TaskData.uptime >>  8) & 0xFF;
    TaskData.Buf[4] = (TaskData.uptime >>  0) & 0xFF;
    
    //Process Vin Data
    TaskData.Vin    = Peripherals_Vin_Get();
    TaskData.Buf[5] = TaskData.Vin >> 8;
    TaskData.Buf[6] = TaskData.Vin & 0xFF;
    
    //Process Analog Temp Data
    TaskData.ATemp  = Peripherals_Analog_Temp_Get(); 
    TaskData.Buf[7] = TaskData.ATemp >> 8;
    TaskData.Buf[8] = TaskData.ATemp & 0xFF;
    
    //Process Digital Temp Data
    TaskData.DTemp   = Peripherals_Digital_Temp_Get();
    TaskData.Buf[9]  = TaskData.DTemp >> 8;
    TaskData.Buf[10] = TaskData.DTemp & 0xFF;
    
    //Process Digital RH Data
    TaskData.DRH     = Peripherals_Digital_RH_Get();
    TaskData.Buf[11] = TaskData.DRH ;
    
    #ifdef DEBUG_ENABLE_WITH_UBRR_VAL
    Debug_Tx_Byte(TaskData.Buf[0]);
    Debug_Tx_Byte(TaskData.Buf[1]);
    Debug_Tx_Byte(TaskData.Buf[2]);
    Debug_Tx_Byte(TaskData.Buf[3]);
    Debug_Tx_Byte(TaskData.Buf[4]);
    Debug_Tx_Byte(TaskData.Buf[5]);
    Debug_Tx_Byte(TaskData.Buf[6]);
    Debug_Tx_Byte(TaskData.Buf[7]);
    Debug_Tx_Byte(TaskData.Buf[8]);
    Debug_Tx_Byte(TaskData.Buf[9]);
    Debug_Tx_Byte(TaskData.Buf[10]);
    Debug_Tx_Byte(TaskData.Buf[11]);
    #endif

    nRF24L01P_WakeUp();
    nRF24L01P_Transmit_Basic(TaskData.Buf, 12);
    nRF24L01P_Deep_Sleep();
    nRF24L01P_Error_Handler();
    Kernel_Task_Sleep(RADIO_TASK_SLEEP_DUR_MS/KER_TICK_TIME);
	  
  }
}

__attribute__((noreturn)) void Task_Sensor(void){
  
  Sensors_Init();
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);
  
  while(1){

    Sensors_Sample_Temp_RH();
    Kernel_Task_Sleep(SENSOR_TASK_SLEEP_DUR_MS/KER_TICK_TIME);
	
  }
}

