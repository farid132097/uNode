

#ifndef  _DEBUG_H_
#define  _DEBUG_H_









#ifndef __ASSEMBLER__           //Only accessible via C
extern void      Debug_Init(uint16_t ubrr_val);
extern void      Debug_Tx_Byte(uint8_t data);
extern void      Debug_Tx_Word(uint16_t data);
extern void      Debug_Tx_Byte_Conf(uint8_t  data);
extern void      Debug_Tx_Word_Conf(uint16_t data);
extern void      Debug_Tx_DWord_Conf(uint32_t data);

extern void      Debug_Tx_From_RAM(uint16_t addr);
extern void      Debug_Tx_RAM_Area(uint16_t strt_addr, uint8_t len);
#endif

#endif


