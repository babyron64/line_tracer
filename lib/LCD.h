#ifndef LCD_HEADER_INCLUDED
#define LCD_HEADER_INCLUDED

#include "stm32f3xx_hal.h"
#include "main.h"

typedef enum _LCD_BOOL {
    LCD_FALSE = 0,
    LCD_TRUE = 1
} LCD_BOOL;

typedef enum _LCD_CONF_ON_OFF {
    LCD_OFF = 0,
    LCD_ON = 1
} LCD_CONF_ON_OFF;

typedef enum _LCD_LINE_TYPE {
    LCD_SINGLE_LINE = 0,
    LCD_DOUBLE_LINE = 1
} LCD_LINE_TYPE;

typedef struct _LCD_Init_TypeDef {
    LCD_LINE_TYPE line_num;
    LCD_CONF_ON_OFF double_height;    
    LCD_CONF_ON_OFF cursor;
    LCD_CONF_ON_OFF blink;
    uint8_t contrast;
} LCD_Init_TypeDef;

typedef struct _LCD_HandleTypeDef {
    I2C_HandleTypeDef *hi2c;
    uint16_t addr;
    LCD_Init_TypeDef Init;
    LCD_BOOL RAM_Addr_Changed;
} LCD_HandleTypeDef;

#define LIM_DIGIT_LEN(Num, Len) ((Num)&((1UL<<(Len))-1))
#define SET_BITS_AT(Bits, Loc, Len) (LIM_DIGIT_LEN(Bits, Len)<<(Loc))

#define LCD_INS_LEN 8U
#define LCD_INS_HEADER(Hdr, Len) SET_BITS_AT(Hdr, LCD_INS_LEN-(Len), Len)

#define LCD_CONTROL_BYTE(Co, Rs) ((LIM_DIGIT_LEN(Co, 1)<<1 | LIM_DIGIT_LEN(Rs, 1)) << 6)

#define LCD_CONTRAST_SET(C) (LCD_INS_HEADER(0b0111U, 4) | SET_BITS_AT(C, 0, 4))

// DL is always 1 -- 8 bit mode
#define LCD_FUNCTION_SET(N, DH, IS) ( \
        LCD_INS_HEADER(0b001U, 3) \
        | SET_BITS_AT(1, 4, 1) \
        | SET_BITS_AT(N, 3, 1) \
        | SET_BITS_AT(DH, 2, 1) \
        | SET_BITS_AT(0, 2, 1) \
        | SET_BITS_AT(IS, 0, 1))

#define LCD_INTERNAL_OSC_FREQ(BS, FR) ( \
        LCD_INS_HEADER(0b0001U, 4) \
        | SET_BITS_AT(BS, 3, 1) \
        | SET_BITS_AT(FR, 0, 3))

#define LCD_POWER_ICON_CONTRAST_CR(Ion, Bon, C) ( \
        LCD_INS_HEADER(0b0101U, 4) \
        | SET_BITS_AT(Ion, 1, 3) \
        | SET_BITS_AT(Bon, 1, 2) \
        | SET_BITS_AT(C>>4, 0, 1))

#define LCD_FOLLOWER_CR(Fon, Rab) ( \
        LCD_INS_HEADER(0b0110U, 4) \
        | SET_BITS_AT(Fon, 3, 1) \
        | SET_BITS_AT(Rab, 0, 3))

#define LCD_CLEAR_DISPLAY() 0b00000001
#define LCD_RETURN_HOME()   0b00000010

#define LCD_DISPLAY_ON_OFF(D, C, B) ( \
        LCD_INS_HEADER(0b00001U, 5) \
        | SET_BITS_AT(D, 2, 1) \
        | SET_BITS_AT(C, 1, 1) \
        | SET_BITS_AT(B, 0, 1))

#define LCD_SET_DDRAM_ADDR(Addr) ( \
        LCD_INS_HEADER(0b1U, 1) \
        | SET_BITS_AT(Addr, 0, 7))

#define LCD_SET_CGRAM_ADDR(Idx, Line) ( \
        LCD_INS_HEADER(0b01, 2) \
        | SET_BITS_AT(Idx, 3, 3) \
        | SET_BITS_AT(Line, 0, 3))

HAL_StatusTypeDef LCD_Init(LCD_HandleTypeDef *hlcd);

void LCD_Write(LCD_HandleTypeDef *hlcd, char* str);

void LCD_Clear(LCD_HandleTypeDef *hlcd);

void LCD_Set_DDRAM_Addr(LCD_HandleTypeDef *hlcd, uint8_t addr);

void LCD_Set_CGRAM(LCD_HandleTypeDef *hlcd, uint8_t idx, uint8_t lines[8]);

#endif // LCD_HEADER_INCLUDED
