

#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "nrf24l01p.h"
#include "sensors.h"
#include "kernel.h"
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
    RGB_Set_State(1,0,0);

    //Blocking delay
    _delay_us(100);

    //Red LED off
    RGB_Set_State(0,0,0);

    //Delay 5000 ms
    Kernel_Task_Sleep(5000/KER_TICK_TIME);

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
    Kernel_Task_Sleep(30000/KER_TICK_TIME);
	
  }
}

void Task_Radio(void){
  
  uint16_t Vin;
  int16_t  ATemp;
  int16_t  DTemp;
  uint16_t DRH;
  uint8_t  buf[32];

  //Radio init with deep sleep
  nRF24L01P_Init();
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);

  while(1){
    
    //Process Vin Data
    Vin = Peripherals_Vin_RawADC_Get();
    buf[0] = Vin >> 8;
    buf[1] = Vin & 0xFF;

    //Process Analog Temp Data
    ATemp = Peripherals_Analog_Temp_Get();
    buf[2] = ATemp >> 8;
    buf[3] = ATemp & 0xFF;
    
    //Process Digital Temp Data
    DTemp = Peripherals_Digital_Temp_Get();
    buf[4] = DTemp >> 8;
    buf[5] = DTemp & 0xFF;

    //Process Digital RH Data
    DRH = Peripherals_Digital_RH_Get();
    buf[6] = DRH >> 8;
    buf[7] = DRH & 0xFF;
    
    Debug_Tx_Byte(buf[0]);
    Debug_Tx_Byte(buf[1]);
    Debug_Tx_Byte(buf[2]);
    Debug_Tx_Byte(buf[3]);
    Debug_Tx_Byte(buf[4]);
    Debug_Tx_Byte(buf[5]);
    Debug_Tx_Byte(buf[6]);
    Debug_Tx_Byte(buf[7]);

    nRF24L01P_WakeUp();
    nRF24L01P_Transmit_Basic(buf, 8);
    nRF24L01P_Deep_Sleep();
    Kernel_Task_Sleep(60000/KER_TICK_TIME);
	  
  }
}

void Task_Sensor(void){
  
  Sensors_Init();
  //Inrush current prevention at startup
  Kernel_Task_Sleep(2000/KER_TICK_TIME);
  
  while(1){

    Sensors_Sample();
    Kernel_Task_Sleep(120000/KER_TICK_TIME);
	
  }
}

