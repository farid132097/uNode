

#ifndef  _RGB_H_
#define  _RGB_H_


#define  RGB_R_DDR  DDRD
#define  RGB_G_DDR  DDRD
#define  RGB_B_DDR  DDRD

#define  RGB_R_PORT PORTD
#define  RGB_G_PORT PORTD
#define  RGB_B_PORT PORTD

#define  RGB_R_PIN  5U
#define  RGB_G_PIN  6U
#define  RGB_B_PIN  7U

void     RGB_Init(void);
void     RGB_Set_State(uint8_t r, uint8_t g, uint8_t b);

#endif


