

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



void Task_Disable_Peripherals(void){
  //ADC and Analog comparator will be
  //turned off automatically by the kernel
}

void Task_RGB_LED(void){
  
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

void Task_Vin_Sense(void){
  
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

void Task_Radio(void){
  
  uint16_t Vin;
  int16_t  ATemp;
  int16_t  DTemp;
  uint16_t DRH;
  uint8_t  buf[32];
  uint32_t uptime;
  
  //Radio init with deep sleep
  nRF24L01P_Init();
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);

  while(1){
    
    uptime = Kernel_Tick_Val_Get();
    Vin    = Peripherals_Vin_Get();
    ATemp  = Peripherals_Analog_Temp_Get(); 
    DTemp  = Peripherals_Digital_Temp_Get();
    DRH    = Peripherals_Digital_RH_Get();

    //Process Vin Data
    buf[0] = DEVICE_NODE_ID;
    buf[1] = (uptime >> 24) & 0xFF;
    buf[2] = (uptime >> 16) & 0xFF;
    buf[3] = (uptime >>  8) & 0xFF;
    buf[4] = (uptime >>  0) & 0xFF;
    
    //Process Vin Data
    buf[5] = Vin >> 8;
    buf[6] = Vin & 0xFF;

    //Process Analog Temp Data
    buf[7] = ATemp >> 8;
    buf[8] = ATemp & 0xFF;
    
    //Process Digital Temp Data
    buf[9]  = DTemp >> 8;
    buf[10] = DTemp & 0xFF;

    //Process Digital RH Data
    buf[11] = DRH ;

    nRF24L01P_WakeUp();
    nRF24L01P_Transmit_Basic(buf, 12);
    nRF24L01P_Deep_Sleep();
    nRF24L01P_Error_Handler();
    Kernel_Task_Sleep(RADIO_TASK_SLEEP_DUR_MS/KER_TICK_TIME);
	  
  }
}

void Task_Sensor(void){
  
  Sensors_Init();
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);
  
  while(1){

    Sensors_Sample_Temp_RH();
    Kernel_Task_Sleep(SENSOR_TASK_SLEEP_DUR_MS/KER_TICK_TIME);
	
  }
}

