
///TX long wire

#include <avr/io.h>
#include <util/delay.h>

#define nRF24L01P_CSN_DDR          DDRB
#define nRF24L01P_CSN_PORT         PORTB
#define nRF24L01P_CSN_bp           2

#define nRF24L01P_CE_DDR           DDRB
#define nRF24L01P_CE_PORT          PORTB
#define nRF24L01P_CE_bp            1

#define nRF24L01P_PACKET_LEN       32

typedef struct nrf24l01p_address_t{
  uint8_t              Own           ;
  uint8_t              Dest          ;
} nrf24l01p_address_t;

typedef struct nrf24l01p_config_t{
  uint16_t             RxTimeout     ;
  uint16_t             RxTicks       ;
  uint8_t              MaxDataLength ;
  uint16_t             MaxRetry      ;
  uint16_t             RetryOccured  ;
} nrf24l01p_config_t;

typedef struct nrf24l01p_packet_t{
  uint8_t              PID           ;
  uint8_t              ACKReq        ;
} nrf24l01p_packet_t;

typedef struct nrf24l01p_t{
  uint8_t              Mode          ;
  uint8_t              TempBuf[2]    ;
  nrf24l01p_address_t  Address       ;
  nrf24l01p_config_t   Config        ;
  nrf24l01p_packet_t   Packet        ;
  uint16_t             ErrorTicks    ;
  uint8_t              Error         ;
} nrf24l01p_t;

nrf24l01p_t nRF24L01P_type;
nrf24l01p_t *nRF24L01P;

void nRF24L01P_Struct_Init(void){
  nRF24L01P=&nRF24L01P_type;
  nRF24L01P->Mode=0x04;
  nRF24L01P->TempBuf[0]=0x00;
  nRF24L01P->TempBuf[1]=0x00;
  nRF24L01P->Address.Own=0x00;
  nRF24L01P->Address.Dest=0x01;
  nRF24L01P->Config.RxTimeout=10;
  nRF24L01P->Config.RxTicks=0;
  nRF24L01P->Config.MaxDataLength=0;
  nRF24L01P->Config.MaxRetry=0;
  nRF24L01P->Config.RetryOccured=0;
  nRF24L01P->Packet.PID=0;
  nRF24L01P->Packet.ACKReq=1;
  nRF24L01P->ErrorTicks=0;
  nRF24L01P->Error=0;
}

void nRF24L01P_CSN_High(void){
  nRF24L01P_CSN_PORT|= (1<<nRF24L01P_CSN_bp);
}

void nRF24L01P_CSN_Low(void){
  nRF24L01P_CSN_PORT&=~(1<<nRF24L01P_CSN_bp);
}

void nRF24L01P_CE_High(void){
  nRF24L01P_CE_PORT|= (1<<nRF24L01P_CE_bp);
}

void nRF24L01P_CE_Low(void){
  nRF24L01P_CE_PORT&=~(1<<nRF24L01P_CE_bp);
}

void nRF24L01P_Enable_GPIO(void){
  DDRB |= (1<<5)|(1<<3)|(1<<2);
  DDRB &=~(1<<4);
  nRF24L01P_CSN_DDR |= (1<<nRF24L01P_CSN_bp);
  nRF24L01P_CE_DDR  |= (1<<nRF24L01P_CE_bp) ;
  nRF24L01P_CSN_High();
  nRF24L01P_CE_Low()  ;
}

void nRF24L01P_Disable_GPIO(void){
  DDRB |= (1<<4);                              
  PORTB&=~((1<<5)|(1<<4)|(1<<3)|(1<<2));
  nRF24L01P_CSN_High(); 
  nRF24L01P_CE_Low() ;  
}

void nRF24L01P_Enable_SPI(void){
  SPCR=(1<<SPE)|(1<<MSTR);                     
  SPSR=(1<<SPI2X);                             
}

void nRF24L01P_Disable_SPI(void){                 
  SPCR=0x00;
  SPSR=0x00;
}

void nRF24L01P_Enable(void){
  nRF24L01P_Enable_GPIO();
  nRF24L01P_Enable_SPI();
}

void nRF24L01P_Disable(void){
  nRF24L01P_Disable_SPI();
  nRF24L01P_Disable_GPIO();
}

void nRF24L01P_Error_Clear(void){
  nRF24L01P->ErrorTicks=0;
  nRF24L01P->Error=0;
}

uint8_t nRF24L01P_Get_Error(void){
  return nRF24L01P->Error;
}

uint8_t nRF24L01P_No_Error(void){
  if(nRF24L01P_Get_Error()==0){
    return 1;
  }else{
    return 0;
  }
}

uint8_t nRF24L01P_Error_Timeout(void){
  _delay_us(1);
  nRF24L01P->ErrorTicks++;
  if(nRF24L01P->ErrorTicks>1000){
    nRF24L01P->ErrorTicks=0;
    nRF24L01P->Error=0x10;
  }
  return nRF24L01P->Error;
}

uint8_t nRF24L01P_SPI_Transfer(uint8_t data){
  uint8_t sts=0;
  if(nRF24L01P_No_Error()){
    SPDR = data;
    nRF24L01P_Error_Clear();
    while(!(SPSR & (1 << SPIF))){
      if(nRF24L01P_Error_Timeout()){
	    sts=0;
	    break;
	  }
    }
    if(nRF24L01P_No_Error()){
      sts=SPDR;
    }else{
      sts=0;
    }
  }else{
    sts=0;
  }
  return sts;
}

uint16_t nRF24L01P_Calcuate_CRC(uint16_t crc, uint8_t data){
  crc=crc^((uint16_t)data<<8);
  for(uint8_t i=0;i<8;i++){
    if(crc & 0x8000){
	  crc=(crc<<1)^0x1021;
	}
    else{
	  crc<<=1;
	}
  }
  return crc;
}

uint16_t nRF24L01P_Calcuate_CRC_Block(uint8_t *buf, uint8_t len){
  uint16_t crc=0;
  for(uint8_t i=0;i<len;i++){
    crc=nRF24L01P_Calcuate_CRC(crc,buf[i]);
  }
  return crc;
}


void nRF24L01P_ReadWrite_Register(uint8_t reg, uint8_t rw, uint8_t *data, uint8_t len){
  if(nRF24L01P_No_Error()){
    nRF24L01P_CSN_Low();
    if(rw==0){
      reg|=0x20;
	  nRF24L01P_SPI_Transfer(reg);
	  for(uint8_t i=0;i<len;i++){
	    nRF24L01P_SPI_Transfer(data[i]);
	  }
    }else{
      nRF24L01P_SPI_Transfer(reg);
      for(uint8_t i=0;i<len;i++){
        data[i]=nRF24L01P_SPI_Transfer(0xFF);
      }
    }
    nRF24L01P_CSN_High();
  }
}

void nRF24L01P_Flush_Transmit_Buffer(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0xE1,0,nRF24L01P->TempBuf,0);
  }
}

void nRF24L01P_Write_Data_To_Transmit_Buffer(uint8_t *data){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0xA0,0,data,32);
  }
}

uint8_t nRF24L01P_Transmit_Buffer_Empty(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P->TempBuf[0]=0;
    nRF24L01P_ReadWrite_Register(0x17,1,nRF24L01P->TempBuf,1);
    if(nRF24L01P->TempBuf[0] & (1<<4)){
      return 1;
    }else{
      return 0;
    }
  }else{
    return 0;
  }
}

void nRF24L01P_Wait_Till_Transmission_Completes(void){
  if(nRF24L01P_No_Error()){
    while(!nRF24L01P_Transmit_Buffer_Empty()){
      _delay_us(100);
    }
  }
}

void nRF24L01P_Flush_Receive_Buffer(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0xE2,0,nRF24L01P->TempBuf,0);
  }
}

void nRF24L01P_Read_Data_From_Receive_Buffer(uint8_t *data){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0x61,1,data,32);
  }
}

uint8_t nRF24L01P_Receive_Buffer_Not_Empty(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P->TempBuf[0]=0;
    nRF24L01P_ReadWrite_Register(0x17,1,nRF24L01P->TempBuf,1);
    if((nRF24L01P->TempBuf[0] & (1<<0))==0){
      return 1;
    }else{
      return 0;
    }
  }else{
    return 0;
  }
}

uint8_t nRF24L01P_Get_Mode(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0x00,1,nRF24L01P->TempBuf,1);
    if(nRF24L01P->TempBuf[0] & (1<<1)){
      if(nRF24L01P->TempBuf[0] & (1<<0)){
	    nRF24L01P->Mode=0x01;
	    return 1; //rx mode
	  }else{
	    nRF24L01P->Mode=0x02;
	    return 2; //tx mode
	  }
    }else{
      nRF24L01P->Mode=0x00;
      return 0; //pwr down
    }
  }else{
    return 0;
  }
}
  
void nRF24L01P_Set_Mode_Sleep(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P->TempBuf[0]=0x00;
    nRF24L01P_ReadWrite_Register(0x00,0,nRF24L01P->TempBuf,1);
    nRF24L01P->Mode=0x00;
  }
}

void nRF24L01P_Set_Mode_Tx(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P->TempBuf[0]=0x72;
    nRF24L01P_CE_Low();
    nRF24L01P_ReadWrite_Register(0x00,0,nRF24L01P->TempBuf,1);
	  nRF24L01P_Flush_Transmit_Buffer();
	  nRF24L01P->Mode=0x02;
  }
}

void nRF24L01P_Set_Mode_Rx(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P->TempBuf[0]=0x73;
    nRF24L01P_CE_High();
    nRF24L01P_ReadWrite_Register(0x00,0,nRF24L01P->TempBuf,1);
	  nRF24L01P->Mode=0x01;
  }
}

void nRF24L01P_ReadModifyWrite(uint8_t reg, uint8_t bit_pos, uint8_t bit_val){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(reg,1,nRF24L01P->TempBuf,1);
    if(bit_val){
      nRF24L01P->TempBuf[0]|=(1<<bit_pos);
    }else{
      nRF24L01P->TempBuf[0]&=~(1<<bit_pos);
    }
    nRF24L01P_ReadWrite_Register(reg,0,nRF24L01P->TempBuf,1);
  }
}

uint8_t nRF24L01P_Get_Channel(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0x05,1,nRF24L01P->TempBuf,1);
    return nRF24L01P->TempBuf[0];
  }else{
    return 0;
  }
}

void nRF24L01P_Set_Channel(uint8_t channel){
  if(nRF24L01P_No_Error()){
    if(channel>125){
      channel=125;
    }
    nRF24L01P->TempBuf[0]=channel;
    nRF24L01P_ReadWrite_Register(0x05,0,nRF24L01P->TempBuf,1);
  }
}

uint8_t nRF24L01P_Get_Speed(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0x06,1,nRF24L01P->TempBuf,1);
    nRF24L01P->TempBuf[1]=(nRF24L01P->TempBuf[0] >> 3) & 0x01;
    nRF24L01P->TempBuf[0]>>=4;
    nRF24L01P->TempBuf[0]&=0x02;
    nRF24L01P->TempBuf[1]|=nRF24L01P->TempBuf[0];
    if      (nRF24L01P->TempBuf[1]==0x02){
      nRF24L01P->TempBuf[0]=0;
    }else if(nRF24L01P->TempBuf[1]==0x01){
      nRF24L01P->TempBuf[0]=1;
    }else if(nRF24L01P->TempBuf[1]==0x00){
      nRF24L01P->TempBuf[0]=2;
    }
    return nRF24L01P->TempBuf[0];
  }else{
    return 0;
  }
}  

void nRF24L01P_Set_Speed(uint8_t index){
  if(nRF24L01P_No_Error()){
    if(index==0){       //250kbps
      nRF24L01P_ReadModifyWrite(0x06,5,1);
      nRF24L01P_ReadModifyWrite(0x06,3,0);
    }
    else if(index==1){  //1Mbps
      nRF24L01P_ReadModifyWrite(0x06,5,0);
      nRF24L01P_ReadModifyWrite(0x06,3,0);
    }
    else if(index==2){  //2Mbps
      nRF24L01P_ReadModifyWrite(0x06,5,0);
      nRF24L01P_ReadModifyWrite(0x06,3,1);
    }else{              //2Mbps
      nRF24L01P_ReadModifyWrite(0x06,5,0);
      nRF24L01P_ReadModifyWrite(0x06,3,1);
    }
  }
}

uint8_t nRF24L01P_Get_Tx_Power(void){
  if(nRF24L01P_No_Error()){
    nRF24L01P_ReadWrite_Register(0x06,1,nRF24L01P->TempBuf,1);
    nRF24L01P->TempBuf[0]>>=1;
    nRF24L01P->TempBuf[0]&=0x03;
    return nRF24L01P->TempBuf[0];
  }else{
    return 0;
  }
}  

void nRF24L01P_Set_Tx_Power(uint8_t index){
  if(nRF24L01P_No_Error()){
    if(index==0){
      nRF24L01P_ReadModifyWrite(0x06,2,0);
      nRF24L01P_ReadModifyWrite(0x06,1,0);
    }
    else if(index==1){
      nRF24L01P_ReadModifyWrite(0x06,2,0);
      nRF24L01P_ReadModifyWrite(0x06,1,1);
    }
    else if(index==2){
      nRF24L01P_ReadModifyWrite(0x06,2,1);
      nRF24L01P_ReadModifyWrite(0x06,1,0);
    }
    else if(index==3){
      nRF24L01P_ReadModifyWrite(0x06,2,1);
      nRF24L01P_ReadModifyWrite(0x06,1,1);
    }else{
      nRF24L01P_ReadModifyWrite(0x06,2,1);
      nRF24L01P_ReadModifyWrite(0x06,1,1);
    }
  }
}

void nRF24L01P_Set_Own_Address(uint8_t addr){
  nRF24L01P->Address.Own=addr;
}

void nRF24L01P_Set_Destination_Address(uint8_t addr){
  nRF24L01P->Address.Dest=addr;
}

void nRF24L01P_Set_Receive_Timeout(uint16_t val){
  nRF24L01P->Config.RxTimeout=val;
}

void nRF24L01P_Set_MaxRetransmission(uint16_t val){
  nRF24L01P->Config.MaxRetry=val;
}

void nRF24L01P_Deep_Sleep(void){
  if(nRF24L01P->Mode != 0x00){
    nRF24L01P_Set_Mode_Sleep();
    nRF24L01P_Disable();
  }
}


void nRF24L01P_WakeUp(void){
  if(nRF24L01P->Mode == 0x00){ 
    nRF24L01P_Enable();
    nRF24L01P_Set_Mode_Rx();
  }
}

void nRF24L01P_Init(void){
  nRF24L01P_Struct_Init();
  nRF24L01P_Enable();
  nRF24L01P->TempBuf[0]=0x00;  nRF24L01P_ReadWrite_Register(0x00,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x00;  nRF24L01P_ReadWrite_Register(0x01,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x03;  nRF24L01P_ReadWrite_Register(0x02,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x01;  nRF24L01P_ReadWrite_Register(0x03,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x00;  nRF24L01P_ReadWrite_Register(0x04,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x02;  nRF24L01P_ReadWrite_Register(0x05,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x26;  nRF24L01P_ReadWrite_Register(0x06,0,nRF24L01P->TempBuf,1); 
  nRF24L01P->TempBuf[0]=0x70;  nRF24L01P_ReadWrite_Register(0x07,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=nRF24L01P_PACKET_LEN;  
  nRF24L01P_ReadWrite_Register(0x11,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=nRF24L01P_PACKET_LEN;  
  nRF24L01P_ReadWrite_Register(0x12,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x00;  nRF24L01P_ReadWrite_Register(0x1C,0,nRF24L01P->TempBuf,1);
  nRF24L01P->TempBuf[0]=0x00;  nRF24L01P_ReadWrite_Register(0x1D,0,nRF24L01P->TempBuf,1);
  nRF24L01P_ReadWrite_Register(0x10,0,(uint8_t*)"ACK00",5);
  nRF24L01P_ReadWrite_Register(0x0A,0,(uint8_t*)"ACK00",5);
  nRF24L01P_ReadWrite_Register(0x0B,0,(uint8_t*)"PIPE1",5);
  nRF24L01P_ReadWrite_Register(0x0C,0,(uint8_t*)"PIPE2",5);
  nRF24L01P_ReadWrite_Register(0x0D,0,(uint8_t*)"PIPE3",5);
  nRF24L01P_ReadWrite_Register(0x0E,0,(uint8_t*)"PIPE4",5);
  nRF24L01P_ReadWrite_Register(0x0F,0,(uint8_t*)"PIPE5",5);
  nRF24L01P_Set_Receive_Timeout(32);
  nRF24L01P_Deep_Sleep();
}

void nRF24L01P_Transmit_Basic(uint8_t *buf, uint8_t len){
  nRF24L01P_Error_Clear();
  nRF24L01P_Set_Mode_Tx();
  buf[nRF24L01P_PACKET_LEN-5]=nRF24L01P->Address.Own;
  buf[nRF24L01P_PACKET_LEN-4]=nRF24L01P->Address.Dest;
  buf[nRF24L01P_PACKET_LEN-3]=len;
  uint16_t temp=nRF24L01P_Calcuate_CRC_Block(buf, 30);
  buf[nRF24L01P_PACKET_LEN-2]=(temp & 0xFF00)>>8;
  buf[nRF24L01P_PACKET_LEN-1]=(temp & 0x00FF);
  nRF24L01P_Write_Data_To_Transmit_Buffer(buf);
  nRF24L01P_CE_High();
  nRF24L01P_Wait_Till_Transmission_Completes();
  nRF24L01P_CE_Low();
}


uint8_t nRF24L01P_Recieve_Basic(uint8_t *buf){
  uint8_t sts=0;
  nRF24L01P_Error_Clear();
  nRF24L01P->Config.RxTicks=0;
  nRF24L01P_Set_Mode_Rx();
  while(nRF24L01P->Config.RxTicks < nRF24L01P->Config.RxTimeout){
    if(nRF24L01P_Receive_Buffer_Not_Empty()){
      nRF24L01P_Read_Data_From_Receive_Buffer(buf);
	  uint16_t rec_crc=buf[nRF24L01P_PACKET_LEN-2];
	  rec_crc<<=8;
	  rec_crc|=buf[nRF24L01P_PACKET_LEN-1];
      uint16_t calc_crc=nRF24L01P_Calcuate_CRC_Block(buf, nRF24L01P_PACKET_LEN-2);
      if(rec_crc == calc_crc){
		sts=1;
		break;
	  }
    }
    _delay_us(100);
    nRF24L01P->Config.RxTicks++;
  }
  return sts;
}

uint8_t nRF24L01P_Transmit_With_ACK(uint8_t *buf, uint8_t len){
  uint8_t sts=0;
  if(nRF24L01P->Packet.ACKReq){
    buf[nRF24L01P_PACKET_LEN-6]=1;
  }else{
    buf[nRF24L01P_PACKET_LEN-6]=0;
  }
  nRF24L01P_Transmit_Basic(buf, len);
  if(nRF24L01P_Recieve_Basic(buf)){
    //if( (nRF24L01P->Address.Own == buf[28])){
      sts=1;
    //}
  }
  return sts;
}


uint8_t nRF24L01P_Recieve_With_ACK(uint8_t *buf){
  uint8_t sts=0;
  if(nRF24L01P_Recieve_Basic(buf)){
    if((buf[nRF24L01P_PACKET_LEN-6]==1) && (nRF24L01P->Address.Own == buf[nRF24L01P_PACKET_LEN-4])){
      _delay_us(500);
	  nRF24L01P_Set_Destination_Address(buf[nRF24L01P_PACKET_LEN-5]);
      nRF24L01P_Transmit_Basic(buf, 2);
      sts=1;
    }
  }
  return sts;
}

/*
uint8_t nRF24L01P_Transmit_No_Retry(uint8_t *buf, uint8_t len){
  uint8_t sts=0;
  if(nRF24L01P_Transmit_With_ACK(buf,len)){
    if(nRF24L01P->RxPacket.PID.Sequence == nRF24L01P->TxPacket.PID.Sequence){
      nRF24L01P->TxPacket.PID.LastPID=nRF24L01P->TxPacket.PID.Sequence;
      if(nRF24L01P->TxPacket.PID.Sequence>=0x09){
        nRF24L01P->TxPacket.PID.Sequence=0;
      }else{
        nRF24L01P->TxPacket.PID.Sequence++;
      }
      sts=1;
    }
  }
  return sts;
}

uint8_t nRF24L01P_Transmit(uint8_t *buf, uint8_t len){
  uint8_t sts=0;
  for(nRF24L01P->Config.RetryOccured=0; nRF24L01P->Config.RetryOccured < nRF24L01P->Config.MaxRetry; nRF24L01P->Config.RetryOccured++){
    sts=nRF24L01P_Transmit_No_Retry(buf, len);
    if(sts==1){
      break;
    }
  }
  return sts;
}

uint8_t nRF24L01P_Recieve(uint8_t *buf){
  uint8_t sts=0;
  if(nRF24L01P_Recieve_With_ACK(buf)){
    if(nRF24L01P->RxPacket.PID.Sequence!=nRF24L01P->RxPacket.PID.LastPID){
      nRF24L01P->RxPacket.PID.LastPID=nRF24L01P->RxPacket.PID.Sequence;
      sts=1;
    }
  }
  return sts;
}

uint8_t nRF24L01P_Block_Transmit(uint8_t *buf, uint16_t len){
  uint8_t  sts=0, TxBuf[32], TxLen=0;
  uint16_t blocks=(len/nRF24L01P->Config.MaxDataLength);
  uint16_t RemainingLen=len;
    
  if( (len % nRF24L01P->Config.MaxDataLength) !=0){
    blocks+=1;
  }
  nRF24L01P->TxPacket.Blocks=blocks-1; //remaining blocks
  for(uint16_t i=0; i<blocks; i++){
    for(uint8_t j=0; j<nRF24L01P->Config.MaxDataLength; j++){
      TxBuf[j]=buf[ (i*nRF24L01P->Config.MaxDataLength) + j ];
    }
    if( RemainingLen > nRF24L01P->Config.MaxDataLength ){
      TxLen = nRF24L01P->Config.MaxDataLength;
    }
    sts = nRF24L01P_Transmit(buf, TxLen);
    if(sts==0){
      break;
    }else{
      RemainingLen-=TxLen;
      if(nRF24L01P->TxPacket.Blocks>0){
        nRF24L01P->TxPacket.Blocks--;
      }
    }    
  }
  return sts;
}

uint8_t nRF24L01P_Block_Receive(uint8_t *buf, uint16_t *len){
  uint8_t sts=0, RxBuf[32], incoming_blocks=0;
  nRF24L01P_Set_Receive_Timeout(15);
  if(nRF24L01P_Recieve(buf)){
    incoming_blocks=nRF24L01P->RxPacket.Blocks;
    if(incoming_blocks==0){
      uint16_t temp_len=nRF24L01P->RxPacket.Config.Length;
      *len=temp_len;
      sts=1;
    }else{
      uint16_t temp_len=nRF24L01P->Config.MaxDataLength;
      nRF24L01P_Set_Receive_Timeout(70);
      for(uint8_t i=0; i<incoming_blocks; i++){
        if(nRF24L01P_Recieve(RxBuf)){
          //fill buffer
          //update index
          //update length
        }else{
          break;
        }
      }
    }
  }
  return sts;
}

///TX long wire

uint8_t len=1;

void setup() {

  nRF24L01P_Init(110);
  nRF24L01P_Set_Own_Address(0x5E);
  nRF24L01P_Set_Destination_Address(0x5D);
  Serial.begin(9600);
  
}

void loop() {
  
  uint8_t data[26]="abcdefghijklmnopqrstuvwxyz";
  if(nRF24L01P_Transmit(data, len)){
    Serial.println("ack"); 
  }else{
    Serial.println("nack");    
  }

  len++;
  if(len>26){
    len=1;
  }
  delay(500);

}*/
