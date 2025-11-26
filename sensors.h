

#ifndef  _SENSORS_H_
#define  _SENSORS_H_

#define  SENSORS_PWR_EN_DDR             DDRD
#define  SENSORS_PWR_EN_PORT            PORTD
#define  SENSORS_PWR_EN_PIN             PIND
#define  SENSORS_PWR_EN_BP              3U

#define  SENSORS_HDC1080_SCL_DDR        DDRC
#define  SENSORS_HDC1080_SCL_PORT       PORTC
#define  SENSORS_HDC1080_SCL_PIN        PINC
#define  SENSORS_HDC1080_SCL_BP         5U

#define  SENSORS_HDC1080_SDA_DDR        DDRC
#define  SENSORS_HDC1080_SDA_PORT       PORTC
#define  SENSORS_HDC1080_SDA_PIN        PINC
#define  SENSORS_HDC1080_SDA_BP         4U

#define  SENSORS_HDC1080_ACK_MAX_RETRY  30
#define  SENSORS_HDC1080_POWER_UP_DELAY 20
#define  SENSORS_HDC1080_CONV_DELAY     6
#define  SENSORS_HDC1080_SCL_FREQ       50000
#define  SENSORS_HDC1080_FULL_BIT       (1000000/SENSORS_HDC1080_SCL_FREQ)
#define  SENSORS_HDC1080_HALF_BIT       (SENSORS_HDC1080_FULL_BIT/4)
#define  SENSORS_HDC1080_QUARTER_BIT    (SENSORS_HDC1080_HALF_BIT/2)

#define  SENSORS_HDC1080_ADDR           0x40

void     Sensors_Init(void);
void     Sensors_I2C_SCL_State_Set(uint8_t state);
void     Sensors_I2C_SDA_State_Set(uint8_t state);
uint8_t  Sensors_I2C_SDA_State_Get(void);
uint8_t  Sensors_I2C_Bus_Idle(void);
void     Sensors_HDC1080_I2C_Start(void);
void     Sensors_HDC1080_I2C_Stop(void);
void     Sensors_HDC1080_I2C_Forced_Stop(void);
void     Sensors_HDC1080_I2C_Send(uint8_t addr);
uint8_t  Sensors_HDC1080_I2C_Receive(void);
void     Sensors_HDC1080_I2C_Check_Ack(void);
void     Sensors_HDC1080_I2C_Send_Ack(void);
uint8_t  Sensors_HDC1080_Slave_Addr_Send(uint8_t addr);

void     Sensors_HDC1080_Config(uint8_t type);
void     Sensors_Sample_Temp_RH(void);

uint8_t  Sensors_HDC1080_Address_Get(void);
uint8_t  Sensors_HDC1080_Status_Get(void);
uint8_t  Sensors_HDC1080_Error_Get(void);
uint8_t  Sensors_HDC1080_Sticky_Error_Get(void);
int16_t  Sensors_HDC1080_Temp_Get(void);
uint16_t Sensors_HDC1080_RH_Get(void);
int16_t  Sensors_NTC_Temp_Get(void);

#endif


