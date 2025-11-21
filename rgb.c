

#include <avr/io.h>
#include "cdefs.h"
#include "rgb.h"


void RGB_Init(void){
    //LEDs are active low
    //Set logic high to turn off
    RGB_R_PORT |= (1<<RGB_R_PIN);
    RGB_G_PORT |= (1<<RGB_G_PIN);
    RGB_B_PORT |= (1<<RGB_B_PIN);
    
    //Declare pins as output
    RGB_R_DDR  |= (1<<RGB_R_PIN);
    RGB_G_DDR  |= (1<<RGB_G_PIN);
    RGB_G_DDR  |= (1<<RGB_G_PIN);
}

void RGB_Set_State(uint8_t r, uint8_t g, uint8_t b){
    if(r == ON){
        RGB_R_PORT &=~ (1<<RGB_R_PIN);
    }
    else{
        RGB_R_PORT |=  (1<<RGB_R_PIN);
    }

    if(g == ON){
        RGB_G_PORT &=~ (1<<RGB_G_PIN);
    }
    else{
        RGB_G_PORT |=  (1<<RGB_G_PIN);
    }

    if(b == ON){
        RGB_B_PORT &=~ (1<<RGB_B_PIN);
    }
    else{
        RGB_B_PORT |=  (1<<RGB_B_PIN);
    }
}