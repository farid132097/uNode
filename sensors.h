

#ifndef  _SENSORS_H_
#define  _SENSORS_H_

#define  SENSORS_PWR_EN_DDR             DDRD
#define  SENSORS_PWR_EN_PORT            PORTD
#define  SENSORS_PWR_EN_PIN             3U

#define  SENSORS_HDC1080_SCL_DDR        DDRC
#define  SENSORS_HDC1080_SCL_PORT       PORTC
#define  SENSORS_HDC1080_SCL_PIN        5U

#define  SENSORS_HDC1080_SDA_DDR        DDRC
#define  SENSORS_HDC1080_SDA_PORT       PORTC
#define  SENSORS_HDC1080_SDA_PIN        4U

#define  SENSORS_HDC1080_CONV_DELAY     20
#define  SENSORS_HDC1080_SCL_FREQ       20000
#define  SENSORS_HDC1080_TWBR_VAL       (((F_CPU/SENSORS_HDC1080_SCL_FREQ)-16)/8)

#define  SENSORS_TWI_START_SENT			0x08 // Start sent
#define  SENSORS_TWI_REP_START_SENT		0x10 // Repeated Start sent

// Master Transmitter Mode
#define  SENSORS_TWI_MT_SLAW_ACK	    0x18 // SLA+W sent and ACK received
#define  SENSORS_TWI_MT_SLAW_NACK		0x20 // SLA+W sent and NACK received
#define  SENSORS_TWI_MT_DATA_ACK		0x28 // DATA sent and ACK received
#define  SENSORS_TWI_MT_DATA_NACK		0x30 // DATA sent and NACK received

// Master Receiver Mode
#define  SENSORS_TWI_MR_SLAR_ACK	    0x40 // SLA+R sent, ACK received
#define  SENSORS_TWI_MR_SLAR_NACK		0x48 // SLA+R sent, NACK received
#define  SENSORS_TWI_MR_DATA_ACK		0x50 // Data received, ACK returned
#define  SENSORS_TWI_MR_DATA_NACK		0x58 // Data received, NACK returned

// Miscellaneous States
#define  SENSORS_TWI_LOST_ARBIT			0x38 // Arbitration has been lost
#define  SENSORS_TWI_NO_RELEVANT_INFO	0xF8 // No relevant information available
#define  SENSORS_TWI_ILLEGAL_START_STOP	0x00 // Illegal START or STOP condition
#define  SENSORS_TWI_SUCCESS			0xFF

void     Sensors_HDC1080_Struct_Init(void);
void     Sensors_Init(void);
void     Sensors_I2C_Enable(void);
void     Sensors_I2C_Disable(void);
void     Sensors_HDC1080_Timeout_Clear(void);
void     Sensors_HDC1080_Error_Clear(void);
uint8_t  Sensors_HDC1080_Timeout(void);
void     Sensors_HDC1080_I2C_Start(void);
void     Sensors_HDC1080_I2C_Stop(void);
uint8_t  Sensors_HDC1080_I2C_Read(void);
void     Sensors_HDC1080_I2C_Write(uint8_t data);
uint8_t  Sensors_HDC1080_I2C_Read_Nack(void);
uint8_t  Sensors_HDC1080_Find_Address(void);
uint16_t Sensors_HDC1080_Read_Reg(uint8_t addr);
void     Sensors_HDC1080_Write_Reg(uint8_t addr, uint16_t val);
void     Sensors_HDC1080_Trigger_Humidity_Measurement(void);
uint16_t Sensors_HDC1080_Read_Humidity_Plain(void);
void     Sensors_Sample(void);

uint8_t  Sensors_HDC1080_Address_Get(void);
uint8_t  Sensors_HDC1080_Status_Get(void);
uint8_t  Sensors_HDC1080_Error_Get(void);
uint8_t  Sensors_HDC1080_Timeout_Error_Get(void);
uint16_t Sensors_HDC1080_RH_Get(void);

#endif


