
#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
#include "nrf24l01p.h"
#include "sensors.h"
#include "kernel.h"
#include "config.h"
#include "cdefs.h"
#include "tasks.h"
#include "debug.h"
#include "rgb.h"

void OTAU_Init(void){
    //B0 as button input
    DDRB &=~ (1<<0);
}

uint8_t OTAU_Button_Pressed(void){
    if(PINB & (1<<0)){
        return FALSE; 
    }
    else{
        return TRUE;
    }
}


