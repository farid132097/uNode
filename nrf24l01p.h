
#define  nRF24L01P_USE_HW_SPI
 
#define  nRF24L01P_SCK_DDR            DDRB
#define  nRF24L01P_SCK_PORT           PORTB
#define  nRF24L01P_SCK_bp             5

#define  nRF24L01P_MISO_DDR           DDRB
#define  nRF24L01P_MISO_PORT          PORTB
#define  nRF24L01P_MISO_bp            4

#define  nRF24L01P_MOSI_DDR           DDRB
#define  nRF24L01P_MOSI_PORT          PORTB
#define  nRF24L01P_MOSI_bp            3

#define  nRF24L01P_CSN_DDR            DDRB
#define  nRF24L01P_CSN_PORT           PORTB
#define  nRF24L01P_CSN_bp             2

#define  nRF24L01P_CE_DDR             DDRB
#define  nRF24L01P_CE_PORT            PORTB
#define  nRF24L01P_CE_bp              1

#define  nRF24L01P_ERROR_FLAGS        10
#define  nRF24L01P_PACKET_LEN         32
#define  nRF24L01P_TX_ACK_RCV_TIMEOUT 5
#define  nRF24L01P_RX_ACK_SEND_DELAY  500

void     nRF24L01P_Struct_Init(void);
void     nRF24L01P_set_SCK_DD(uint8_t state);
void     nRF24L01P_set_MISO_DD(uint8_t state);
void     nRF24L01P_set_MOSI_DD(uint8_t state);
void     nRF24L01P_set_CSN_DD(uint8_t state);
void     nRF24L01P_set_CE_DD(uint8_t state);

void     nRF24L01P_set_SCK(uint8_t state);
void     nRF24L01P_set_MISO(uint8_t state);
void     nRF24L01P_set_MOSI(uint8_t state);
void     nRF24L01P_set_CSN(uint8_t state);
void     nRF24L01P_set_CE(uint8_t state);

void     nRF24L01P_GPIO_Enable(void);
void     nRF24L01P_GPIO_Disable(void);
void     nRF24L01P_SPI_Enable(void);
void     nRF24L01P_SPI_Disable(void);
void     nRF24L01P_Enable(void);
void     nRF24L01P_Disable(void);

void     nRF24L01P_Error_Clear(void);
void     nRF24L01P_Error_Clear_Ticks(void);
void     nRF24L01P_Error_Clear_Buf(void);
void     nRF24L01P_Error_Clear_All(void);
void     nRF24L01P_Error_Set(uint8_t val);
uint8_t  nRF24L01P_Error_Get(void);
uint8_t  nRF24L01P_Error_Get_Index(uint8_t index);
uint8_t  nRF24L01P_Error_Timeout(uint16_t ticks);
uint8_t  nRF24L01P_Error_Free(void);

uint16_t nRF24L01P_CRC(uint16_t crc, uint8_t data);
uint16_t nRF24L01P_CRC_Block(uint8_t *buf, uint8_t len);

uint8_t  nRF24L01P_SPI_Transfer(uint8_t data);

void     nRF24L01P_ReadWrite_Register(uint8_t reg, uint8_t rw, uint8_t *data, uint8_t len);
void     nRF24L01P_ReadModifyWrite_Register( uint8_t reg, uint8_t bit_pos, uint8_t bit_val );

void     nRF24L01P_Flush_Tx_Buf(void);
void     nRF24L01P_Flush_Rx_Buf(void);

void     nRF24L01P_Write_Data_Tx_Buf(uint8_t *data);
void     nRF24L01P_Read_Data_Rx_Buf(uint8_t *data);

uint8_t  nRF24L01P_Tx_Buf_Empty(void);
uint8_t  nRF24L01P_Rx_Buf_Not_Empty(void);
uint8_t  nRF24L01P_Wait_Tx_Complete(void);

void     nRF24L01P_Mode_Set_DeepSleep(void);
void     nRF24L01P_Mode_Set_Sleep(void);
void     nRF24L01P_Mode_Set_Rx(void);
void     nRF24L01P_Mode_Set_Tx(void);
void     nRF24L01P_Mode_Set(uint8_t Mode);
uint8_t  nRF24L01P_Mode_Get(void);

void     nRF24L01P_Channel_Set(uint8_t channel);
uint8_t  nRF24L01P_Channel_Get(void);

void     nRF24L01P_Speed_Set(uint8_t index);
uint8_t  nRF24L01P_Speed_Get(void);

void     nRF24L01P_Tx_Power_Set(uint8_t index);
uint8_t  nRF24L01P_Tx_Power_Get(void);

void     nRF24L01P_Own_Addr_Set(uint8_t addr);
uint8_t  nRF24L01P_Own_Addr_Get(void);

void     nRF24L01P_Dest_Addr_Set(uint8_t addr);
uint8_t  nRF24L01P_Dest_Addr_Get(void);

void     nRF24L01P_RxTicks_Set(uint16_t val);
uint16_t nRF24L01P_RxTicks_Get(void);

void     nRF24L01P_RxTimeout_Set(uint16_t val);
uint16_t nRF24L01P_RxTimeout_Get(void);

void     nRF24L01P_MaxRetransmission_Set(uint16_t val);
uint16_t nRF24L01P_MaxRetransmission_Get(void);

void     nRF24L01P_Tx_Basic(uint8_t *buf, uint8_t len);
uint8_t  nRF24L01P_Rx_Basic(uint8_t *buf);

uint8_t  nRF24L01P_Tx_With_Ack(uint8_t *buf, uint8_t len);
uint8_t  nRF24L01P_Rx_With_Ack(uint8_t *buf);

void     nRF24L01P_Init(void);



