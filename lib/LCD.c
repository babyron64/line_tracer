#include "stm32f3xx_hal.h"

#include "LCD.h"

#define LCD_SEND_TIMEOUT 500 // ms
#define LCD_CMD_STABLE_WAIT 1 // ms
#define LCD_DATA_STABLE_WAIT 1 //ms

static void LCD_Send_Cmd(LCD_HandleTypeDef *hlcd, uint8_t cmd);
static void LCD_Write_Byte(LCD_HandleTypeDef *hlcd, uint8_t data);
static void LCD_Set_CGRAM_Addr(LCD_HandleTypeDef *hlcd, uint8_t idx);
static uint8_t LCD_Calc_Code(char c);

static void LCD_Send_Cmd(LCD_HandleTypeDef *hlcd, uint8_t cmd) {
    uint8_t cmd_pair[2] = {
        LCD_CONTROL_BYTE(0, 0),
        cmd
    };

    HAL_I2C_Master_Transmit(hlcd->hi2c, hlcd->addr, cmd_pair, 2, LCD_SEND_TIMEOUT);
    HAL_Delay(LCD_CMD_STABLE_WAIT);
    return;
}

static void LCD_Write_Byte(LCD_HandleTypeDef *hlcd, uint8_t data) {
    uint8_t cmd_pair[2] = {
        LCD_CONTROL_BYTE(0, 1),
        data
    };

    HAL_I2C_Master_Transmit(hlcd->hi2c, hlcd->addr, cmd_pair, 2, LCD_SEND_TIMEOUT);
    hlcd->RAM_Addr_Changed = LCD_FALSE;
    HAL_Delay(LCD_DATA_STABLE_WAIT);
    return;
}

void LCD_Write(LCD_HandleTypeDef *hlcd, char* str) {
    while(*str) {
        LCD_Write_Byte(hlcd, LCD_Calc_Code(*str));
        str++;
    }
    return;
}

void LCD_Clear(LCD_HandleTypeDef *hlcd) {
    LCD_Send_Cmd(hlcd, LCD_CLEAR_DISPLAY());
    hlcd->RAM_Addr_Changed = LCD_TRUE;
    return;
}

void LCD_Set_DDRAM_Addr(LCD_HandleTypeDef *hlcd, uint8_t addr) {
    LCD_Send_Cmd(hlcd, LCD_SET_DDRAM_ADDR(addr));
    hlcd->RAM_Addr_Changed = LCD_TRUE;
    return;
}

static void LCD_Set_CGRAM_Addr(LCD_HandleTypeDef *hlcd, uint8_t idx) {
    LCD_Send_Cmd(hlcd, LCD_SET_CGRAM_ADDR(idx, 0U));
    hlcd->RAM_Addr_Changed = LCD_TRUE;
    return;
}

void LCD_Set_CGRAM(LCD_HandleTypeDef *hlcd, uint8_t idx, uint8_t lines[8]) {
    LCD_Set_CGRAM_Addr(hlcd, idx);
    for (int i=0; i<8; i++)
        LCD_Write_Byte(hlcd, LIM_DIGIT_LEN(lines[i], 5));
    return;
}

static uint8_t LCD_Calc_Code(char c) {
    if ((0x0 <= c && c <= 0x7)  || // CG RAM character.
        (0x20 <= c && c <= 0x7d)) // a-z, A-Z and some characters other than '~'.
        return (uint8_t)c;
    else
        return (uint8_t)'.';
}

HAL_StatusTypeDef LCD_Init(LCD_HandleTypeDef *hlcd) {
    if (hlcd->Init.line_num & hlcd->Init.double_height)
        return HAL_ERROR;

    // wait for LCD set up.
    HAL_Delay(100);

    LCD_Send_Cmd(hlcd, LCD_FUNCTION_SET(hlcd->Init.line_num, hlcd->Init.double_height, 0U));
    LCD_Send_Cmd(hlcd, LCD_FUNCTION_SET(hlcd->Init.line_num, hlcd->Init.double_height, 1U));
    LCD_Send_Cmd(hlcd, LCD_INTERNAL_OSC_FREQ(0U, 0b100U));
    LCD_Send_Cmd(hlcd, LCD_CONTRAST_SET(hlcd->Init.contrast));
    LCD_Send_Cmd(hlcd, LCD_POWER_ICON_CONTRAST_CR(0U, 1U, hlcd->Init.contrast));
    LCD_Send_Cmd(hlcd, LCD_FOLLOWER_CR(1U, 0b100U));

    // wait for LCD power stable
    HAL_Delay(300);

    LCD_Send_Cmd(hlcd, LCD_FUNCTION_SET(hlcd->Init.line_num, hlcd->Init.double_height, 0U));
    LCD_Send_Cmd(hlcd, LCD_CLEAR_DISPLAY());
    LCD_Send_Cmd(hlcd, LCD_DISPLAY_ON_OFF(1U, hlcd->Init.cursor, hlcd->Init.blink));

    hlcd->RAM_Addr_Changed = LCD_TRUE;

    return HAL_OK;
}
