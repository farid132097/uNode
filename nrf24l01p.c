

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24l01p.h"


typedef struct nrf24l01p_sp_alloc_t{
  uint8_t              CRCMSB        ;
  uint8_t              CRCLSB        ;
  uint8_t              Len           ;
  uint8_t              Dest          ;
  uint8_t              Own           ;
  uint8_t              Ack           ;
  uint8_t              PID           ;
  uint8_t              CRCLen        ;
  uint8_t              Blocks        ;
} nrf24l01p_sp_alloc_t;

typedef struct nrf24l01p_address_t{
  uint8_t              Own           ;
  uint8_t              Dest          ;
} nrf24l01p_address_t;

typedef struct nrf24l01p_config_t{
  uint16_t             RxTimeout     ;
  uint16_t             RxTicks       ;
  uint8_t              MaxDataLen    ;
  uint16_t             MaxRetry      ;
  uint16_t             RetryOccured  ;
} nrf24l01p_config_t;

typedef struct nrf24l01p_packet_t{
  uint8_t              TxPID         ;
  uint8_t              RxPID         ;
  uint8_t              LastRxPID     ;
  uint8_t              AckReq        ;
} nrf24l01p_packet_t;

typedef struct nrf24l01p_blocks_t{
  uint8_t              Remaining     ;
  uint8_t              FailedPos     ;
} nrf24l01p_blocks_t;

typedef struct nrf24l01p_t{
  uint8_t              Mode          ;
  uint8_t              Enable        ;
  uint8_t              TempBuf[2]    ;
  nrf24l01p_address_t  Address       ;
  nrf24l01p_config_t   Config        ;
  nrf24l01p_packet_t   Packet        ;
  nrf24l01p_blocks_t   Blocks        ;
  nrf24l01p_sp_alloc_t SpaceAlloc    ;
  uint16_t             ErrorTicks    ;
  uint8_t              Error         ;
  uint8_t              Errors[nRF24L01P_ERROR_FLAGS];
} nrf24l01p_t;

nrf24l01p_t nRF24L01P;



/* initialize structure of the driver */

void nRF24L01P_Struct_Init(void){
  nRF24L01P.Mode = 0x00;
  nRF24L01P.Enable = 0x01;
  nRF24L01P.TempBuf[0] = 0x00;
  nRF24L01P.TempBuf[1] = 0x00;
  nRF24L01P.Address.Own = 0x01;
  nRF24L01P.Address.Dest = 0x02;
  nRF24L01P.Config.RxTicks = 0;
  nRF24L01P.Config.RxTimeout = 10;
  nRF24L01P.Config.MaxDataLen = 20;
  nRF24L01P.Config.MaxRetry = 10;
  nRF24L01P.Config.RetryOccured = 0;
  nRF24L01P.Packet.TxPID = 0x01;
  nRF24L01P.Packet.RxPID = 0x00;
  nRF24L01P.Packet.LastRxPID = 0x00;
  nRF24L01P.Packet.AckReq = 1;
  nRF24L01P.Blocks.Remaining = 0;
  nRF24L01P.Blocks.FailedPos = 0;
  nRF24L01P.SpaceAlloc.CRCLSB = nRF24L01P_PACKET_LEN-1;
  nRF24L01P.SpaceAlloc.CRCMSB = nRF24L01P_PACKET_LEN-2;
  nRF24L01P.SpaceAlloc.CRCLen = nRF24L01P_PACKET_LEN-2;
  nRF24L01P.SpaceAlloc.Len = nRF24L01P_PACKET_LEN-3;
  nRF24L01P.SpaceAlloc.Dest = nRF24L01P_PACKET_LEN-4;
  nRF24L01P.SpaceAlloc.Own = nRF24L01P_PACKET_LEN-5;
  nRF24L01P.SpaceAlloc.Ack = nRF24L01P_PACKET_LEN-6;
  nRF24L01P.SpaceAlloc.PID = nRF24L01P_PACKET_LEN-7;
  nRF24L01P.SpaceAlloc.Blocks = nRF24L01P_PACKET_LEN-8;
  nRF24L01P.ErrorTicks = 0;
  nRF24L01P.Error = 0;
  for(uint8_t i = 0; i < nRF24L01P_ERROR_FLAGS; i++){
    nRF24L01P.Errors[0] = 0;
  }
}




/* set data direction of associated gpio */

void nRF24L01P_Set_SCK_DD(uint8_t state){
  if(state){
    nRF24L01P_SCK_DDR |= (1<<nRF24L01P_SCK_bp);
  }else{
    nRF24L01P_SCK_DDR &=~(1<<nRF24L01P_SCK_bp);
  }
}

void nRF24L01P_Set_MISO_DD(uint8_t state){
  if(state){
    nRF24L01P_MISO_DDR |= (1<<nRF24L01P_MISO_bp);
  }else{
    nRF24L01P_MISO_DDR &=~(1<<nRF24L01P_MISO_bp);
  }
}

void nRF24L01P_Set_MOSI_DD(uint8_t state){
  if(state){
    nRF24L01P_MOSI_DDR |= (1<<nRF24L01P_MOSI_bp);
  }else{
    nRF24L01P_MOSI_DDR &=~(1<<nRF24L01P_MOSI_bp);
  }
}


void nRF24L01P_Set_CSN_DD(uint8_t state){
  if(state){
    nRF24L01P_CSN_DDR |= (1<<nRF24L01P_CSN_bp);
  }else{
    nRF24L01P_CSN_DDR &=~(1<<nRF24L01P_CSN_bp);
  }
}

void nRF24L01P_Set_CE_DD(uint8_t state){
  if(state){
    nRF24L01P_CE_DDR |= (1<<nRF24L01P_CE_bp);
  }else{
    nRF24L01P_CE_DDR &=~(1<<nRF24L01P_CE_bp);
  }
}




/* set output state of associated gpio */

void nRF24L01P_Set_SCK(uint8_t state){
  if(state){
    nRF24L01P_SCK_PORT |= (1<<nRF24L01P_SCK_bp);
  }else{
    nRF24L01P_SCK_PORT &=~(1<<nRF24L01P_SCK_bp);
  }
}

void nRF24L01P_Set_MISO(uint8_t state){
  if(state){
    nRF24L01P_MISO_PORT |= (1<<nRF24L01P_MISO_bp);
  }else{
    nRF24L01P_MISO_PORT &=~(1<<nRF24L01P_MISO_bp);
  }
}

void nRF24L01P_Set_MOSI(uint8_t state){
  if(state){
    nRF24L01P_MOSI_PORT |= (1<<nRF24L01P_MOSI_bp);
  }else{
    nRF24L01P_MOSI_PORT &=~(1<<nRF24L01P_MOSI_bp);
  }
}

void nRF24L01P_Set_CSN(uint8_t state){
  if(state){
    nRF24L01P_CSN_PORT |= (1<<nRF24L01P_CSN_bp);
  }else{
    nRF24L01P_CSN_PORT &=~(1<<nRF24L01P_CSN_bp);
  }
}

void nRF24L01P_Set_CE(uint8_t state){
  if(state){
    nRF24L01P_CE_PORT |= (1<<nRF24L01P_CE_bp);
  }else{
    nRF24L01P_CE_PORT &=~(1<<nRF24L01P_CE_bp);
  }
}




/* configuration of gpio for different states */

void nRF24L01P_GPIO_Enable(void){
  nRF24L01P_Set_SCK_DD(1);
  nRF24L01P_Set_MISO_DD(0);
  nRF24L01P_Set_MOSI_DD(1);
  nRF24L01P_Set_CSN_DD(1);
  nRF24L01P_Set_CE_DD(1);
  nRF24L01P_Set_CSN(1);
  nRF24L01P_Set_CE(0);
  #ifdef nRF24L01P_USE_HW_SPI
  DDRB |= (1<<2);
  #endif
}

void nRF24L01P_GPIO_Disable(void){
  nRF24L01P_Set_SCK_DD(1);
  nRF24L01P_Set_MISO_DD(1);
  nRF24L01P_Set_MOSI_DD(1);
  nRF24L01P_Set_CSN_DD(1);
  nRF24L01P_Set_CE_DD(1);
  nRF24L01P_Set_SCK(0);
  nRF24L01P_Set_MISO(0);
  nRF24L01P_Set_MOSI(0);
  nRF24L01P_Set_CSN(1);
  nRF24L01P_Set_CE(0);
}




/* configuration of spi for different states */

void nRF24L01P_SPI_Enable(void){
  #ifdef nRF24L01P_USE_HW_SPI
  SPCR = (1<<SPE)|(1<<MSTR);                     
  SPSR = (1<<SPI2X);
  SPSR = 0;
  #endif
}

void nRF24L01P_SPI_Disable(void){
  #ifdef nRF24L01P_USE_HW_SPI
  SPCR = 0x00;                     
  SPSR = 0x00;
  #endif
}




/* configuration of gpio & spi for different states */

void nRF24L01P_Enable(void){
  nRF24L01P_GPIO_Enable();
  nRF24L01P_SPI_Enable();
  nRF24L01P.Enable = 1;
}

void nRF24L01P_Disable(void){
  nRF24L01P_SPI_Disable();
  nRF24L01P_GPIO_Disable();
  nRF24L01P.Enable = 0;
}



/* error handling */

void nRF24L01P_Error_Clear(void){
  nRF24L01P.Error = 0;
}

void nRF24L01P_Error_Clear_Ticks(void){
  nRF24L01P.ErrorTicks = 0;
}

void nRF24L01P_Error_Clear_Buf(void){
  for(uint8_t i = 0; i < nRF24L01P_ERROR_FLAGS; i++){
    nRF24L01P.Errors[i] = 0;
  }
}

void nRF24L01P_Error_Clear_All(void){
  nRF24L01P_Error_Clear();
  nRF24L01P_Error_Clear_Ticks();
  nRF24L01P_Error_Clear_Buf();
}

void nRF24L01P_Error_Set(uint8_t val){
  nRF24L01P.Error = val;
}

uint8_t nRF24L01P_Error_Get(void){
  return nRF24L01P.Error;
}

uint8_t nRF24L01P_Error_Get_Index(uint8_t index){
  return nRF24L01P.Errors[index];
}

uint8_t nRF24L01P_Error_Timeout(uint16_t ticks){
  _delay_us(100);
  nRF24L01P.ErrorTicks++;
  if(nRF24L01P.ErrorTicks>ticks){
	return 1;
  }
  else{
    return 0;
  }
}

uint8_t nRF24L01P_Error_Free(void){
  if(nRF24L01P_Error_Get() == 0){
    return 1;
  }
  else{
    return 0;
  }
}



/* crc calcultions and validation */

uint16_t nRF24L01P_CRC(uint16_t crc, uint8_t data){
  crc=crc^((uint16_t)data<<8);
  for(uint8_t i = 0; i < 8; i++){
    if(crc & 0x8000){
	  crc = (crc<<1)^0x1021;
	}
    else{
	  crc <<= 1;
	}
  }
  return crc;
}

uint16_t nRF24L01P_CRC_Block(uint8_t *buf, uint8_t len){
  uint16_t crc = 0;
  for(uint8_t i = 0; i < len; i++){
    crc = nRF24L01P_CRC(crc,buf[i]);
  }
  return crc;
}



/* spi transfer */

uint8_t nRF24L01P_SPI_Transfer(uint8_t data){
  if(nRF24L01P_Error_Free()){
    SPDR = data;
    uint8_t i = 0;
    while(!(SPSR & (1 << SPIF))){
	  _delay_us(10);
	  i++;
	  if(i > 200){
	    nRF24L01P_Error_Set(0x01);
	    return 0;
	  }
    }
	return SPDR;
  }else{
    return 0;
  }
}


/* register read write */

void nRF24L01P_ReadWrite_Register(uint8_t reg, uint8_t rw, uint8_t *data, uint8_t len){
  if(nRF24L01P_Error_Free()){
    if(nRF24L01P.Enable == 0){
      nRF24L01P_Enable();
    }
    nRF24L01P_Set_CSN(0);
    if(rw == 0){
      reg |= 0x20;
	  nRF24L01P_SPI_Transfer(reg);
	  for(uint8_t i = 0; i < len; i++){
	    nRF24L01P_SPI_Transfer(data[i]);
	  }
    }else{
      nRF24L01P_SPI_Transfer(reg);
      for(uint8_t i = 0; i < len; i++){
        data[i] = nRF24L01P_SPI_Transfer(0xFF);
      }
    }
    nRF24L01P_Set_CSN(1);
  }
}

void nRF24L01P_ReadModifyWrite_Register( uint8_t reg, uint8_t bit_pos, uint8_t bit_val ){
  if(nRF24L01P_Error_Free()){
    nRF24L01P_ReadWrite_Register( reg, 1, nRF24L01P.TempBuf, 1);
    if(bit_val){
      nRF24L01P.TempBuf[0]|=(1<<bit_pos);
    }else{
      nRF24L01P.TempBuf[0]&=~(1<<bit_pos);
    }
    nRF24L01P_ReadWrite_Register( reg, 0, nRF24L01P.TempBuf, 1 );
  }
}


/* tx and rx buffer flush */

void nRF24L01P_Flush_Tx_Buf(void){
  nRF24L01P_ReadWrite_Register( 0xE1, 0, nRF24L01P.TempBuf, 0 );
  if( !nRF24L01P_Error_Free() ){
    nRF24L01P_Error_Set(0x02);
  }
}

void nRF24L01P_Flush_Rx_Buf(void){
  nRF24L01P_ReadWrite_Register( 0xE2, 0, nRF24L01P.TempBuf, 0 );
  if(!nRF24L01P_Error_Free()){
    nRF24L01P_Error_Set(0x03);
  }
}



/* read and write in tx and rx fifo */

void nRF24L01P_Write_Data_Tx_Buf(uint8_t *data){
  nRF24L01P_ReadWrite_Register( 0xA0, 0, data, nRF24L01P_PACKET_LEN );
  if( !nRF24L01P_Error_Free() ){
    nRF24L01P_Error_Set(0x04);
  }
}

void nRF24L01P_Read_Data_Rx_Buf(uint8_t *data){
  nRF24L01P_ReadWrite_Register( 0x61, 1, data, nRF24L01P_PACKET_LEN );
  if( !nRF24L01P_Error_Free() ){
    nRF24L01P_Error_Set(0x05);
  }
}



/* read tx and rx fifo status */

uint8_t nRF24L01P_Tx_Buf_Empty(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P.TempBuf[0] = 0;
    nRF24L01P_ReadWrite_Register( 0x17, 1, nRF24L01P.TempBuf, 1);
    if(nRF24L01P.TempBuf[0] & (1<<4)){
      return 1;
    }else{
      return 0;
    }
  }else{
    return 0;
  }
}


uint8_t nRF24L01P_Rx_Buf_Not_Empty(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P.TempBuf[0] = 0;
    nRF24L01P_ReadWrite_Register( 0x17, 1, nRF24L01P.TempBuf, 1);
    if((nRF24L01P.TempBuf[0] & (1<<0)) == 0){
      return 1;
    }else{
      return 0;
    }
  }else{
    return 0;
  }
}


uint8_t nRF24L01P_Wait_Tx_Complete(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P_Error_Clear_Ticks();
    while( !nRF24L01P_Tx_Buf_Empty() ){
	  if(nRF24L01P_Error_Timeout(100)){
	    nRF24L01P_Error_Set(0x06);
	    return 0;
	  }
    }
	return 1;
  }else{
    return 0;
  }
}



/* set Modes */

void nRF24L01P_Mode_Set_DeepSleep(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P.TempBuf[0] = 0x00;
    nRF24L01P_ReadWrite_Register( 0x00, 0, nRF24L01P.TempBuf, 1 );
    nRF24L01P.Mode = 0x00;
  }
  nRF24L01P_Disable();
}

void nRF24L01P_Mode_Set_Sleep(void){
  if(nRF24L01P.Enable == 0){
    nRF24L01P_Enable();
  }
  if(nRF24L01P_Error_Free()){
    nRF24L01P.TempBuf[0] = 0x00;
	nRF24L01P_Set_CE(0);
    nRF24L01P_ReadWrite_Register( 0x00, 0, nRF24L01P.TempBuf, 1 );
    nRF24L01P.Mode = 0x01;
  }
}

void nRF24L01P_Mode_Set_Rx(void){
  if(nRF24L01P.Enable == 0){
    nRF24L01P_Enable();
  }
  if(nRF24L01P_Error_Free()){
    nRF24L01P.TempBuf[0] = 0x73;
    nRF24L01P_Set_CE(1);
    nRF24L01P_ReadWrite_Register( 0x00, 0, nRF24L01P.TempBuf, 1 );
	nRF24L01P.Mode = 0x02;
  }
}

void nRF24L01P_Mode_Set_Tx(void){
  if(nRF24L01P.Enable == 0){
    nRF24L01P_Enable();
  }
  if(nRF24L01P_Error_Free()){
    nRF24L01P.TempBuf[0] = 0x72;
    nRF24L01P_Set_CE(0);
    nRF24L01P_ReadWrite_Register( 0x00, 0, nRF24L01P.TempBuf, 1 );
	nRF24L01P_Set_CE(1);
	nRF24L01P.Mode = 0x03;
  }
}

void nRF24L01P_Mode_Set(uint8_t Mode){
  if(nRF24L01P_Error_Free()){
    if(Mode == 0){
	  nRF24L01P_Mode_Set_DeepSleep();
	}else if(Mode == 1){
	  nRF24L01P_Mode_Set_Sleep();
	}else if(Mode == 2){
	  nRF24L01P_Mode_Set_Rx();
	}else if(Mode == 3){
	  nRF24L01P_Mode_Set_Tx();
	}else{
	  nRF24L01P_Mode_Set_Rx();
	}
  }
}

uint8_t nRF24L01P_Mode_Get(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P_ReadWrite_Register( 0x00, 1, nRF24L01P.TempBuf, 1 );
    if(nRF24L01P.TempBuf[0] & (1<<1)){
      if(nRF24L01P.TempBuf[0] & (1<<0)){
	    nRF24L01P.Mode = 0x02;
	    return 2; //rx Mode
	  }else{
	    nRF24L01P.Mode = 0x03;
	    return 3; //tx Mode
	  }
    }else{
      nRF24L01P.Mode = 0x01;
      return 1;   //pwr down
    }
  }else{
    return 0;
  }
}



void nRF24L01P_Channel_Set(uint8_t channel){
  if(nRF24L01P_Error_Free()){
    if(channel > 120){
      channel = 120;
    }
    nRF24L01P.TempBuf[0] = channel;
    nRF24L01P_ReadWrite_Register( 0x05, 0, nRF24L01P.TempBuf, 1 );
  }
}

uint8_t nRF24L01P_Channel_Get(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P_ReadWrite_Register( 0x05, 1, nRF24L01P.TempBuf, 1 );
    return nRF24L01P.TempBuf[0];
  }else{
    return 0;
  }
}



void nRF24L01P_Speed_Set(uint8_t index){
  if(index == 0){       //250kbps
    nRF24L01P_ReadModifyWrite_Register( 0x06, 5, 1 );
    nRF24L01P_ReadModifyWrite_Register( 0x06, 3, 0 );
  }
  else if(index == 1){  //1Mbps
    nRF24L01P_ReadModifyWrite_Register( 0x06, 5, 0 );
    nRF24L01P_ReadModifyWrite_Register( 0x06, 3, 0 );
  }
  else if(index == 2){  //2Mbps
    nRF24L01P_ReadModifyWrite_Register( 0x06, 5, 0 );
    nRF24L01P_ReadModifyWrite_Register( 0x06, 3, 1 );
  }else{                //2Mbps
    nRF24L01P_ReadModifyWrite_Register( 0x06, 5, 0 );
    nRF24L01P_ReadModifyWrite_Register( 0x06, 3, 1 );
  }
}

uint8_t nRF24L01P_Speed_Get(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P_ReadWrite_Register( 0x06, 1, nRF24L01P.TempBuf, 1 );
    nRF24L01P.TempBuf[1] = (nRF24L01P.TempBuf[0] >> 3) & 0x01;
    nRF24L01P.TempBuf[0] >>= 4;
    nRF24L01P.TempBuf[0] &= 0x02;
    nRF24L01P.TempBuf[1] |= nRF24L01P.TempBuf[0];
    if      (nRF24L01P.TempBuf[1] == 0x02){
      nRF24L01P.TempBuf[0] = 0;
    }else if(nRF24L01P.TempBuf[1] == 0x01){
      nRF24L01P.TempBuf[0] = 1;
    }else if(nRF24L01P.TempBuf[1] == 0x00){
      nRF24L01P.TempBuf[0] = 2;
    }
    return nRF24L01P.TempBuf[0];
  }else{
    return 0;
  }
}



void nRF24L01P_Tx_Power_Set(uint8_t index){
  if(nRF24L01P_Error_Free()){
    if(index == 0){
      nRF24L01P_ReadModifyWrite_Register( 0x06, 2, 0 );
      nRF24L01P_ReadModifyWrite_Register( 0x06, 1, 0 );
    }
    else if(index == 1){
      nRF24L01P_ReadModifyWrite_Register( 0x06, 2, 0 );
      nRF24L01P_ReadModifyWrite_Register( 0x06, 1, 1 );
    }
    else if(index == 2){
      nRF24L01P_ReadModifyWrite_Register( 0x06, 2, 1 );
      nRF24L01P_ReadModifyWrite_Register( 0x06, 1, 0 );
    }
    else if(index == 3){
      nRF24L01P_ReadModifyWrite_Register( 0x06, 2, 1 );
      nRF24L01P_ReadModifyWrite_Register( 0x06, 1, 1 );
    }else{
      nRF24L01P_ReadModifyWrite_Register( 0x06, 2, 1 );
      nRF24L01P_ReadModifyWrite_Register( 0x06, 1, 1 );
    }
  }
}

uint8_t nRF24L01P_Tx_Power_Get(void){
  if(nRF24L01P_Error_Free()){
    nRF24L01P_ReadWrite_Register( 0x06, 1, nRF24L01P.TempBuf, 1 );
    nRF24L01P.TempBuf[0] >>= 1;
    nRF24L01P.TempBuf[0] &= 0x03;
    return nRF24L01P.TempBuf[0];
  }else{
    return 0;
  }
}  

void nRF24L01P_Own_Addr_Set(uint8_t addr){
  nRF24L01P.Address.Own=addr;
}

uint8_t nRF24L01P_Own_Addr_Get(void){
  return nRF24L01P.Address.Own;
}

void nRF24L01P_Dest_Addr_Set(uint8_t addr){
  nRF24L01P.Address.Dest=addr;
}

uint8_t nRF24L01P_Dest_Addr_Get(void){
  return nRF24L01P.Address.Dest;
}

void nRF24L01P_RxTicks_Set(uint16_t val){
  nRF24L01P.Config.RxTicks=val;
}

uint16_t nRF24L01P_RxTicks_Get(void){
  return nRF24L01P.Config.RxTicks;
}

void nRF24L01P_RxTimeout_Set(uint16_t val){
  nRF24L01P.Config.RxTimeout=val;
}

uint16_t nRF24L01P_RxTimeout_Get(void){
  return nRF24L01P.Config.RxTimeout;
}

void nRF24L01P_MaxRetransmission_Set(uint16_t val){
  nRF24L01P.Config.MaxRetry=val;
}

uint16_t nRF24L01P_MaxRetransmission_Get(void){
  return nRF24L01P.Config.MaxRetry;
}

void nRF24L01P_Tx_Basic(uint8_t *buf, uint8_t len){
  nRF24L01P_Error_Clear();
  nRF24L01P_Flush_Tx_Buf();
  buf[nRF24L01P.SpaceAlloc.Blocks] = nRF24L01P.Blocks.Remaining;      //24
  buf[nRF24L01P.SpaceAlloc.PID   ] = nRF24L01P.Packet.TxPID;          //25
  buf[nRF24L01P.SpaceAlloc.Ack   ] = nRF24L01P.Packet.AckReq;         //26
  buf[nRF24L01P.SpaceAlloc.Own   ] = nRF24L01P.Address.Own;           //27
  buf[nRF24L01P.SpaceAlloc.Dest  ] = nRF24L01P.Address.Dest;          //28
  buf[nRF24L01P.SpaceAlloc.Len   ] = len;                             //29
  uint16_t temp = nRF24L01P_CRC_Block( buf, nRF24L01P.SpaceAlloc.CRCLen );
  buf[nRF24L01P.SpaceAlloc.CRCMSB] = (temp & 0xFF00)>>8;              //30
  buf[nRF24L01P.SpaceAlloc.CRCLSB] = (temp & 0x00FF);                 //31
  nRF24L01P_Write_Data_Tx_Buf(buf);
  nRF24L01P_Mode_Set(3);                                              //tx
  nRF24L01P_Wait_Tx_Complete(); 
  //nRF24L01P_Mode_Set(2);                                            //rx
}


uint8_t nRF24L01P_Rx_Basic(uint8_t *buf){
  uint8_t sts=0;
  nRF24L01P_Error_Clear();
  nRF24L01P_RxTicks_Set(0);
  while(nRF24L01P_RxTicks_Get() < nRF24L01P_RxTimeout_Get()){
    if(nRF24L01P_Rx_Buf_Not_Empty()){
      nRF24L01P_Read_Data_Rx_Buf(buf);
	  uint16_t rec_crc=buf[nRF24L01P.SpaceAlloc.CRCMSB];               //30
	  rec_crc<<=8;
	  rec_crc|=buf[nRF24L01P.SpaceAlloc.CRCLSB];                       //31
      uint16_t calc_crc=nRF24L01P_CRC_Block(buf, nRF24L01P.SpaceAlloc.CRCLen);
      if(rec_crc == calc_crc){
	    nRF24L01P.Packet.RxPID = buf[nRF24L01P.SpaceAlloc.PID];        //25
	    nRF24L01P.Blocks.Remaining = buf[nRF24L01P.SpaceAlloc.Blocks]; //24
		sts=1;
		break;
	  }
    }else{
      _delay_us(100);
      nRF24L01P.Config.RxTicks++;
	}
  }
  return sts;
}


uint8_t nRF24L01P_Tx_With_Ack(uint8_t *buf, uint8_t len){
  uint8_t  sts = 0, ack_buf[32];
  uint16_t timeout_mem = nRF24L01P_RxTimeout_Get();
  nRF24L01P_RxTimeout_Set(nRF24L01P_TX_ACK_RCV_TIMEOUT);
  nRF24L01P_Tx_Basic(buf, len);
  if( nRF24L01P.Packet.AckReq ){                    //need to improve ack type
    if( nRF24L01P_Rx_Basic(ack_buf) ){
      if( nRF24L01P_Own_Addr_Get() == ack_buf[nRF24L01P.SpaceAlloc.Dest] ){
        sts=1;
      }
    }
  }
  nRF24L01P_RxTimeout_Set(timeout_mem);
  return sts;
}

uint8_t nRF24L01P_Rx_With_Ack(uint8_t *buf){
  uint8_t sts=0, ack_buf[32]="~^~";
  if( nRF24L01P_Rx_Basic(buf) ){
    if( ((buf[nRF24L01P.SpaceAlloc.Ack] & 1)==1) && ( nRF24L01P_Own_Addr_Get() == buf[nRF24L01P.SpaceAlloc.Dest]) ){ //check ack type
      _delay_us(nRF24L01P_RX_ACK_SEND_DELAY);                                                                                               //check delay
	  nRF24L01P_Dest_Addr_Set(buf[nRF24L01P.SpaceAlloc.Own]);
	  nRF24L01P.Packet.TxPID = buf[nRF24L01P.SpaceAlloc.PID];
	  nRF24L01P.Blocks.Remaining = buf[nRF24L01P.SpaceAlloc.Blocks];
      nRF24L01P_Tx_Basic(ack_buf, 3);
      sts=1;
    }
  }
  return sts;
}

void nRF24L01P_Init(void){
  nRF24L01P_Struct_Init();
  nRF24L01P_Enable();
  nRF24L01P.TempBuf[0] = 0x00;  nRF24L01P_ReadWrite_Register( 0x00, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x00;  nRF24L01P_ReadWrite_Register( 0x01, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x03;  nRF24L01P_ReadWrite_Register( 0x02, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x01;  nRF24L01P_ReadWrite_Register( 0x03, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x00;  nRF24L01P_ReadWrite_Register( 0x04, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x02;  nRF24L01P_ReadWrite_Register( 0x05, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x26;  nRF24L01P_ReadWrite_Register( 0x06, 0, nRF24L01P.TempBuf, 1 ); 
  nRF24L01P.TempBuf[0] = 0x70;  nRF24L01P_ReadWrite_Register( 0x07, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = nRF24L01P_PACKET_LEN;  
  nRF24L01P_ReadWrite_Register( 0x11, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = nRF24L01P_PACKET_LEN;  
  nRF24L01P_ReadWrite_Register( 0x12, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x00;  nRF24L01P_ReadWrite_Register( 0x1C, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P.TempBuf[0] = 0x00;  nRF24L01P_ReadWrite_Register( 0x1D, 0, nRF24L01P.TempBuf, 1 );
  nRF24L01P_ReadWrite_Register( 0x10, 0, (uint8_t*)"ACK00", 5 );
  nRF24L01P_ReadWrite_Register( 0x0A, 0, (uint8_t*)"ACK00", 5 );
  nRF24L01P_Speed_Set(4);
  nRF24L01P_Mode_Set(0);
}



