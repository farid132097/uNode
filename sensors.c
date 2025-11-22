
#include <avr/io.h>
#include <util/delay.h>
#include "sensors.h"

#define  SENSOR_ERROR_I2C_START_TIMEOUT      0x01
#define  SENSOR_ERROR_I2C_START_NOT_SENT     0x02
#define  SENSOR_ERROR_I2C_ADDR_SEND_TIMEOUT  0x03
#define  SENSOR_ERROR_I2C_ADDR_NACK          0x04


typedef struct hdc1080_t{
  uint8_t  Status;
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
    SENSORS_PWR_EN_PORT |= (1<<SENSORS_PWR_EN_PIN);
    //Declare as output pin
    SENSORS_PWR_EN_DDR  |= (1<<SENSORS_PWR_EN_PIN);
    
    //SCL as input, internal pull-up disabled, HW I2C
    SENSORS_HDC1080_SCL_PORT &=~ (1<<SENSORS_HDC1080_SCL_PIN);
    SENSORS_HDC1080_SCL_DDR  &=~ (1<<SENSORS_HDC1080_SCL_PIN);

    //SDA as input, internal pull-up disabled, HW I2C
    SENSORS_HDC1080_SDA_PORT &=~ (1<<SENSORS_HDC1080_SDA_PIN);
    SENSORS_HDC1080_SDA_DDR  &=~ (1<<SENSORS_HDC1080_SDA_PIN);
}

void Sensors_I2C_Enable(void){
    TWBR = SENSORS_HDC1080_TWBR_VAL;
    TWCR = (1<<TWEN);
}

void Sensors_I2C_Disable(void){
    TWCR = 0x00;
    TWBR = 0x00;
}

void Sensors_HDC1080_Timeout_Clear(void){
    HDC1080.LoopCnt = 0;
    HDC1080.TimeoutError = 0;
}

void Sensors_HDC1080_Error_Clear(void){
    HDC1080.Error = 0;
}

void Sensors_HDC1080_Error_Reset(void){
    Sensors_HDC1080_Timeout_Clear();
    Sensors_HDC1080_Error_Clear();
}

uint8_t Sensors_HDC1080_Timeout(void){
    _delay_us(10);
    uint8_t sts = 0;
    HDC1080.LoopCnt++;
    if(HDC1080.LoopCnt > 100){
      HDC1080.LoopCnt = 0;
	  sts = 1;
    }
    return sts;
}

void Sensors_HDC1080_I2C_Start(void){
    if(HDC1080.Error == 0){
        TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTA);
        Sensors_HDC1080_Timeout_Clear();
        while( !(TWCR & (1<<TWINT)) ){
            if(Sensors_HDC1080_Timeout()){
                HDC1080.Error = SENSOR_ERROR_I2C_START_TIMEOUT;
	            return;
	        }
        }
        HDC1080.Status = (TWSR & 0xF8);
        if(HDC1080.Status != SENSORS_TWI_START_SENT){
            HDC1080.Error = SENSOR_ERROR_I2C_START_NOT_SENT;
        }
    }
}

void Sensors_HDC1080_I2C_Stop(void){
    if(HDC1080.Error == 0){
        TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
    }
}

void Sensors_HDC1080_I2C_Forced_Stop_Clear_Error(void){
    if(HDC1080.Error != 0){
        TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
        HDC1080.StickyError = HDC1080.Error;
        HDC1080.Error = 0;
    }
}

void Sensors_HDC1080_I2C_Send_Address(uint8_t addr){
    if(HDC1080.Error == 0){
        TWDR = addr << 1;
        TWCR = (1<<TWEN)|(1<<TWINT);
        Sensors_HDC1080_Timeout_Clear();
        while( !(TWCR & (1<<TWINT)) ){
            if(Sensors_HDC1080_Timeout()){
	            HDC1080.Error = SENSOR_ERROR_I2C_ADDR_SEND_TIMEOUT;
	            return;
	        }
        }
        HDC1080.Status = (TWSR & 0xF8);
        if(HDC1080.Status != SENSORS_TWI_MT_SLAW_ACK){
            HDC1080.Error = SENSOR_ERROR_I2C_ADDR_NACK;
        }
    }
}


//need to check later
uint8_t Sensors_HDC1080_I2C_Read(void){
    if(HDC1080.TimeoutError==0){
        TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
        Sensors_HDC1080_Timeout_Clear();
        while(!(TWCR & (1<<TWINT))){
            if(Sensors_HDC1080_Timeout()){
	            HDC1080.TimeoutError=0xF2;
	            break;
	        }
        }
        HDC1080.Status=(TWSR & 0xF8);
        return TWDR;
    }
    else{
        return 0;
    }
}

void Sensors_HDC1080_I2C_Write(uint8_t data){
    if(HDC1080.Error == 0){
        TWDR = data;
        TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
        Sensors_HDC1080_Timeout_Clear();
        while(!(TWCR & (1<<TWINT))){
            if(Sensors_HDC1080_Timeout()){
	            HDC1080.TimeoutError=0xF1;
	            break;
	        }
        }
        HDC1080.Status=(TWSR & 0xF8);
    }
}

uint8_t Sensors_HDC1080_I2C_Read_Nack(void){
    if(HDC1080.TimeoutError==0){
        TWCR=(1<<TWEN)|(1<<TWINT);
        Sensors_HDC1080_Timeout_Clear();
        while(!(TWCR & (1<<TWINT))){
            if(Sensors_HDC1080_Timeout()){
	            HDC1080.TimeoutError=0xF3;
	            break;
	        }
        }
        HDC1080.Status=(TWSR & 0xF8);
        return TWDR;
    }
    else{
        return 0;
    }
}
uint8_t Sensors_HDC1080_Find_Address(void){
    uint8_t addr=0;
    for(uint8_t i=0;i<127;i++){
        Sensors_HDC1080_I2C_Start();
	    Sensors_HDC1080_I2C_Write(i<<1);
	    Sensors_HDC1080_I2C_Stop();
	    if(HDC1080.TimeoutError==0){
	        if(HDC1080.Status==SENSORS_TWI_MT_SLAW_ACK){
	            addr=i;
	            HDC1080.Address=i;
	            break;
	        }
	    }
        else{
	        addr=0;
	    }
    }
    return addr;
}

uint16_t Sensors_HDC1080_Read_Reg(uint8_t addr){
    uint16_t val=0;
    Sensors_HDC1080_I2C_Start();
    Sensors_HDC1080_I2C_Write(HDC1080.Address<<1);
    if(HDC1080.Status==SENSORS_TWI_MT_SLAW_ACK){
        Sensors_HDC1080_I2C_Write(addr);
	    if(HDC1080.Status==SENSORS_TWI_MT_DATA_ACK){
	        Sensors_HDC1080_I2C_Stop();
	        Sensors_HDC1080_I2C_Start();
	        Sensors_HDC1080_I2C_Write((HDC1080.Address<<1)|0x01);
            if(HDC1080.Status==SENSORS_TWI_MR_SLAR_ACK){
	            val=Sensors_HDC1080_I2C_Read();
		        if(HDC1080.Status==SENSORS_TWI_MR_DATA_ACK){
		            val<<=8;
		            val|=Sensors_HDC1080_I2C_Read_Nack();
		            if(HDC1080.Status==SENSORS_TWI_MR_DATA_NACK){
		                HDC1080.Error=0x00;
		            }
                    else{
		                HDC1080.Error=0x05;
		            }
		        }
                else{
		            HDC1080.Error=0x04;
		        }
	        }
            else{
	            HDC1080.Error=0x03;
	        }
	    }
        else{
	        HDC1080.Error=0x02;
	    }
    }
    else{
        HDC1080.Error=0x01;
    }
    Sensors_HDC1080_I2C_Stop();
    if(HDC1080.Error!=0x00){
        val=0;
    }
    return val;
}


void Sensors_HDC1080_Write_Reg(uint8_t addr, uint16_t val){
    Sensors_HDC1080_I2C_Start();
    Sensors_HDC1080_I2C_Write(HDC1080.Address<<1);
    if(HDC1080.Status==SENSORS_TWI_MT_SLAW_ACK){
        Sensors_HDC1080_I2C_Write(addr);
	    if(HDC1080.Status==SENSORS_TWI_MT_DATA_ACK){
	        Sensors_HDC1080_I2C_Write(val>>8);
	        if(HDC1080.Status==SENSORS_TWI_MT_DATA_ACK){
	            Sensors_HDC1080_I2C_Write(val&0xFF);
		        if(HDC1080.Status==SENSORS_TWI_MT_DATA_ACK){
		            HDC1080.Error=0x00;
		        }
                else{
		            HDC1080.Error=0x14;
		        }
	        }
            else{
	            HDC1080.Error=0x13;
	        }
	    }
        else{
	        HDC1080.Error=0x12;
	    }
    }
    else{
        HDC1080.Error=0x11;
    }
    Sensors_HDC1080_I2C_Stop();
}

void Sensors_HDC1080_Trigger_Humidity_Measurement(void){
    Sensors_HDC1080_I2C_Start();
    Sensors_HDC1080_I2C_Write(HDC1080.Address<<1);
    if(HDC1080.Status==SENSORS_TWI_MT_SLAW_ACK){
        Sensors_HDC1080_I2C_Write(0x01);
	    if(HDC1080.Status==SENSORS_TWI_MT_DATA_ACK){
	        HDC1080.Error=0x00;
	    }
        else{
	        HDC1080.Error=0x22;
	    }
    }
    else{
        HDC1080.Error=0x21;
    }
    Sensors_HDC1080_I2C_Stop();
}

uint16_t Sensors_HDC1080_Read_Humidity_Plain(void){
    uint32_t val=0;
    Sensors_HDC1080_Timeout_Clear();
    _delay_ms(SENSORS_HDC1080_CONV_DELAY);
    Sensors_HDC1080_I2C_Start();
    Sensors_HDC1080_I2C_Write((HDC1080.Address<<1)|0x01);
    val=Sensors_HDC1080_I2C_Read();
    val<<=8;
    val|=Sensors_HDC1080_I2C_Read_Nack();
    Sensors_HDC1080_I2C_Stop();
    if(Sensors_HDC1080_Timeout_Error_Get()==0){
        val*=100;
        val/=65535;
        HDC1080.RH = (uint16_t)val;
    }
    else{
        val=0;
    }
    return val;
}



void Sensors_Sample(void){
    //Enable Power to sensor module
    SENSORS_PWR_EN_PORT &=~ (1<<SENSORS_PWR_EN_PIN);
    //Enable I2C hardware
    Sensors_I2C_Enable();
    //Wait until sensor is ready
    _delay_ms(20);
    
    Sensors_HDC1080_Error_Reset();
    Sensors_HDC1080_I2C_Start();
    Sensors_HDC1080_I2C_Send_Address(0x40);
    Sensors_HDC1080_I2C_Stop();

    //Clear errors, send forced stop if necessary
    Sensors_HDC1080_I2C_Forced_Stop_Clear_Error();
    //Disable I2C hardware
    Sensors_I2C_Disable();
    //Disable Power to sensor module
    SENSORS_PWR_EN_PORT |=  (1<<SENSORS_PWR_EN_PIN);
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

uint8_t Sensors_HDC1080_Timeout_Error_Get(void){
  return HDC1080.TimeoutError;
}

uint16_t Sensors_HDC1080_RH_Get(void){
    return HDC1080.RH;
}