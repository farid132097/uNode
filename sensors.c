
#include <avr/io.h>
#include <util/delay.h>
#include "sensors.h"
#include "cdefs.h"


#define  SENSOR_STATUS_I2C_SLA_W_ACK_REC     0x01
#define  SENSOR_STATUS_I2C_SLA_W_NACK_REC    0x02
#define  SENSOR_STATUS_I2C_SLA_R_ACK_REC     0x03
#define  SENSOR_STATUS_I2C_SLA_R_NACK_REC    0x04

#define  SENSOR_ERROR_I2C_ADDR_W_NACK        0x01
#define  SENSOR_ERROR_I2C_ADDR_R_NACK        0x02
#define  SENSOR_ERROR_I2C_DATA_READ_NACK     0x03
#define  SENSOR_ERROR_I2C_DATA_WRITE_NACK    0x04


typedef struct hdc1080_t{
    uint8_t  Status;
    uint8_t  AckStatus;
    uint8_t  SCLState;
    uint8_t  SDAState;
    uint8_t  Address;
    uint16_t LoopCnt;
    int16_t  Temp;
    uint16_t RH;
    uint8_t  TimeoutError;
    uint8_t  Error;
    uint8_t  StickyError;
}hdc1080_t;

hdc1080_t HDC1080;

void Sensors_HDC1080_Struct_Init(void){
    HDC1080.Status = 0;
    HDC1080.Address = 0;
    HDC1080.LoopCnt = 0;
    HDC1080.Temp = 0;
    HDC1080.RH = 0;
    HDC1080.TimeoutError = 0;
    HDC1080.Error = 0;
    HDC1080.StickyError = 0;
}

void Sensors_Init(void){
    Sensors_HDC1080_Struct_Init();
    //Gated with PMOS
    //Output logic high to turn off
    SENSORS_PWR_EN_PORT |= (1<<SENSORS_PWR_EN_BP);
    //Declare as output pin
    SENSORS_PWR_EN_DDR  |= (1<<SENSORS_PWR_EN_BP);
    
    //SCL as input, internal pull-up disabled, HW I2C
    SENSORS_HDC1080_SCL_PORT &=~ (1<<SENSORS_HDC1080_SCL_BP);
    SENSORS_HDC1080_SCL_DDR  &=~ (1<<SENSORS_HDC1080_SCL_BP);

    //SDA as input, internal pull-up disabled, HW I2C
    SENSORS_HDC1080_SDA_PORT &=~ (1<<SENSORS_HDC1080_SDA_BP);
    SENSORS_HDC1080_SDA_DDR  &=~ (1<<SENSORS_HDC1080_SDA_BP);
}

void Sensors_I2C_SCL_State_Set(uint8_t state){
    if(state == LOGIC_HIGH){
        SENSORS_HDC1080_SCL_DDR  &=~ (1<<SENSORS_HDC1080_SCL_BP);
        HDC1080.SCLState = LOGIC_HIGH;
    }
    else if(state == LOGIC_LOW){
        SENSORS_HDC1080_SCL_DDR  |=  (1<<SENSORS_HDC1080_SCL_BP);
        HDC1080.SCLState = LOGIC_LOW;
    }
}

void Sensors_I2C_SDA_State_Set(uint8_t state){
    if(state == LOGIC_HIGH){
        SENSORS_HDC1080_SDA_DDR  &=~ (1<<SENSORS_HDC1080_SDA_BP);
        HDC1080.SDAState = LOGIC_HIGH;
    }
    else if(state == LOGIC_LOW){
        SENSORS_HDC1080_SDA_DDR  |=  (1<<SENSORS_HDC1080_SDA_BP);
        HDC1080.SDAState = LOGIC_LOW;
    }
}

uint8_t Sensors_I2C_SDA_State_Get(void){
    if(SENSORS_HDC1080_SDA_DDR & (1<<SENSORS_HDC1080_SDA_BP)){
        SENSORS_HDC1080_SDA_DDR  &=~ (1<<SENSORS_HDC1080_SDA_BP);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
    }
    if(SENSORS_HDC1080_SDA_PIN & (1<<SENSORS_HDC1080_SDA_BP)){
        return LOGIC_HIGH;
    }
    else{
        return LOGIC_LOW;
    }
}

uint8_t Sensors_I2C_Bus_Idle(void){
    if( ((SENSORS_HDC1080_SCL_DDR & (1<<SENSORS_HDC1080_SCL_BP)) && (SENSORS_HDC1080_SDA_DDR & (1<<SENSORS_HDC1080_SDA_BP))) ){
        return TRUE;
    }
    return FALSE;
}

void Sensors_HDC1080_I2C_Start(void){
    if(HDC1080.Error == 0){
        Sensors_I2C_SCL_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
        Sensors_I2C_SDA_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
        Sensors_I2C_SDA_State_Set(LOGIC_LOW);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
        Sensors_I2C_SCL_State_Set(LOGIC_LOW);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
    }
}

void Sensors_HDC1080_I2C_Stop(void){
    if(HDC1080.Error == 0){
        //if SCL logic high
        if( !(SENSORS_HDC1080_SCL_DDR & (1<<SENSORS_HDC1080_SCL_BP)) ){
            Sensors_I2C_SCL_State_Set(LOGIC_LOW);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
        }
        //if SDA logic high
        if( !(SENSORS_HDC1080_SDA_DDR & (1<<SENSORS_HDC1080_SDA_BP)) ){
            Sensors_I2C_SDA_State_Set(LOGIC_LOW);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
        }
        Sensors_I2C_SCL_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
        Sensors_I2C_SDA_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
    }
}

void Sensors_HDC1080_I2C_Forced_Stop_Clear_Error(void){
    if(HDC1080.Error != 0){
        if ( Sensors_I2C_Bus_Idle() == FALSE ){
            Sensors_I2C_SCL_State_Set(LOGIC_LOW);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
            Sensors_I2C_SDA_State_Set(LOGIC_LOW);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
            Sensors_I2C_SCL_State_Set(LOGIC_HIGH);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
            Sensors_I2C_SDA_State_Set(LOGIC_HIGH);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
        }
        HDC1080.StickyError = HDC1080.Error;
        HDC1080.Error = 0;
    }
}

void Sensors_HDC1080_I2C_Send(uint8_t addr){
    if(HDC1080.Error == 0){
        for(uint8_t i=0;i<8;i++){
            if(addr & 0x80){
                Sensors_I2C_SDA_State_Set(LOGIC_HIGH);
            }
            else{
                Sensors_I2C_SDA_State_Set(LOGIC_LOW);
            }
            _delay_us(SENSORS_HDC1080_HALF_BIT);
            Sensors_I2C_SCL_State_Set(LOGIC_HIGH);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
            Sensors_I2C_SCL_State_Set(LOGIC_LOW);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
            addr <<= 1;
        }
    }
}

uint8_t Sensors_HDC1080_I2C_Receive(void){
    uint8_t val = 0;
    if(HDC1080.Error == 0){
        //Set SDA as input
        Sensors_I2C_SDA_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_HALF_BIT);
        for(uint8_t i=0;i<8;i++){
            val <<= 1;
            if(Sensors_I2C_SDA_State_Get() == LOGIC_HIGH){
                val |= 1;
            }
            Sensors_I2C_SCL_State_Set(LOGIC_HIGH);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
            Sensors_I2C_SCL_State_Set(LOGIC_LOW);
            _delay_us(SENSORS_HDC1080_HALF_BIT);
        }
    }
    return val;
}

void Sensors_HDC1080_I2C_Check_Ack(void){
    if(HDC1080.Error == 0){
        //SDA input mode
        Sensors_I2C_SDA_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
        Sensors_I2C_SCL_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
        if(Sensors_I2C_SDA_State_Get() == LOGIC_LOW){
            HDC1080.AckStatus = TRUE;
        }
        else{
            HDC1080.AckStatus = FALSE;
        }
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
        Sensors_I2C_SCL_State_Set(LOGIC_LOW);
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
    }
}

void Sensors_HDC1080_I2C_Send_Ack(void){
    if(HDC1080.Error == 0){
        //SDA input mode
        Sensors_I2C_SDA_State_Set(LOGIC_LOW);
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
        Sensors_I2C_SCL_State_Set(LOGIC_HIGH);
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
        Sensors_I2C_SCL_State_Set(LOGIC_LOW);
        _delay_us(SENSORS_HDC1080_QUARTER_BIT);
    }
}




void Sensors_Sample(void){
    uint32_t rh_val = 0;
    //Enable Power to sensor module
    SENSORS_PWR_EN_PORT &=~ (1<<SENSORS_PWR_EN_BP);
    //Wait until sensor is ready
    _delay_ms(SENSORS_HDC1080_CONV_DELAY);
    
    //Clear all errors
    HDC1080.Error = 0;

    //Config sensor
    for(uint8_t i = 0; i<50; i++){
        Sensors_HDC1080_I2C_Start();
        Sensors_HDC1080_I2C_Send( SENSORS_HDC1080_ADDR << 1 );
        Sensors_HDC1080_I2C_Check_Ack();
        if(HDC1080.AckStatus == FALSE){
            Sensors_HDC1080_I2C_Stop();
            if(i == 49){
                HDC1080.Error = SENSOR_ERROR_I2C_ADDR_W_NACK;
            }
        }
        else{
            break;
        }
    }
    Sensors_HDC1080_I2C_Send(0x02);
    Sensors_HDC1080_I2C_Check_Ack();
    Sensors_HDC1080_I2C_Send(0x00);
    Sensors_HDC1080_I2C_Check_Ack();
    Sensors_HDC1080_I2C_Send(0x00);
    Sensors_HDC1080_I2C_Check_Ack();
    Sensors_HDC1080_I2C_Stop();
    
    //Trigger RH measurement
    Sensors_HDC1080_I2C_Start();
    Sensors_HDC1080_I2C_Send(SENSORS_HDC1080_ADDR << 1);
    Sensors_HDC1080_I2C_Check_Ack();
    Sensors_HDC1080_I2C_Send(0x01);
    Sensors_HDC1080_I2C_Check_Ack();
    Sensors_HDC1080_I2C_Stop();

    //Read RH
    
    for(uint8_t i = 0; i<50; i++){
        Sensors_HDC1080_I2C_Start();
        Sensors_HDC1080_I2C_Send( (SENSORS_HDC1080_ADDR << 1) | 1);
        Sensors_HDC1080_I2C_Check_Ack();
        if(HDC1080.AckStatus == FALSE){
            Sensors_HDC1080_I2C_Stop();
        }
        else{
            break;
        }
    }
    rh_val = Sensors_HDC1080_I2C_Receive();
    Sensors_HDC1080_I2C_Send_Ack();
    rh_val <<= 8;
    rh_val |= Sensors_HDC1080_I2C_Receive();
    Sensors_HDC1080_I2C_Check_Ack();
    Sensors_HDC1080_I2C_Stop();

    if(HDC1080.Error == 0){
        rh_val*=100;
        rh_val/=65535;
        HDC1080.RH = (uint16_t)rh_val;
    }
    else{
        HDC1080.RH = 0;
    }

    //Clear errors, send forced stop if necessary
    Sensors_HDC1080_I2C_Forced_Stop_Clear_Error();
    //Disable Power to sensor module
    SENSORS_PWR_EN_PORT |=  (1<<SENSORS_PWR_EN_BP);
}




uint8_t Sensors_HDC1080_Address_Get(void){
  return HDC1080.Address;
}

uint8_t Sensors_HDC1080_Status_Get(void){
  return HDC1080.Status;
}

uint8_t Sensors_HDC1080_Error_Get(void){
  return HDC1080.Error;
}

uint8_t Sensors_HDC1080_Sticky_Error_Get(void){
  return HDC1080.StickyError;
}

uint16_t Sensors_HDC1080_RH_Get(void){
    return HDC1080.RH;
}