/*****************************************************************************
*
* File                : ssd1331.c
* Hardware Environment: Raspberry Pi
* Build Environment   : GCC
* Version             : V1.0.7
* Author              : Yehui
*
*              (c) Copyright 2005-2017, WaveShare
*                   http://www.waveshare.com
*                   http://www.waveshare.net   
*                      All Rights Reserved
*              
******************************************************************************/

#include <bcm2835.h>
#include <stdio.h>
#include "ssd1331.h"

char buffer[OLED_WIDTH * OLED_HEIGHT * 2];

void command(char cmd) {
    bcm2835_gpio_write(DC, LOW);
    bcm2835_spi_transfer(cmd);
}

void SSD1331_begin()
{
    bcm2835_gpio_fsel(RST, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(DC, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                  //The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);  //The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                     //The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);     //the default

    bcm2835_gpio_write(RST, HIGH);
    bcm2835_delay(10);
    bcm2835_gpio_write(RST, LOW);
    bcm2835_delay(10);
    bcm2835_gpio_write(RST, HIGH);

    command(DISPLAY_OFF);          //Display Off
    command(SET_CONTRAST_A);       //Set contrast for color A
    command(0xFF);                     //145 0x91
    command(SET_CONTRAST_B);       //Set contrast for color B
    command(0xFF);                     //80 0x50
    command(SET_CONTRAST_C);       //Set contrast for color C
    command(0xFF);                     //125 0x7D
    command(MASTER_CURRENT_CONTROL);//master current control
    command(0x06);                     //6
    command(SET_PRECHARGE_SPEED_A);//Set Second Pre-change Speed For ColorA
    command(0x64);                     //100
    command(SET_PRECHARGE_SPEED_B);//Set Second Pre-change Speed For ColorB
    command(0x78);                     //120
    command(SET_PRECHARGE_SPEED_C);//Set Second Pre-change Speed For ColorC
    command(0x64);                     //100
    command(SET_REMAP);            //set remap & data format
    command(0x72);                     //0x72              
    command(SET_DISPLAY_START_LINE);//Set display Start Line
    command(0x0);
    command(SET_DISPLAY_OFFSET);   //Set display offset
    command(0x0);
    command(NORMAL_DISPLAY);       //Set display mode
    command(SET_MULTIPLEX_RATIO);  //Set multiplex ratio
    command(0x3F);
    command(SET_MASTER_CONFIGURE); //Set master configuration
    command(0x8E);
    command(POWER_SAVE_MODE);      //Set Power Save Mode
    command(0x00);                     //0x00
    command(PHASE_PERIOD_ADJUSTMENT);//phase 1 and 2 period adjustment
    command(0x31);                     //0x31
    command(DISPLAY_CLOCK_DIV);    //display clock divider/oscillator frequency
    command(0xF0);
    command(SET_PRECHARGE_VOLTAGE);//Set Pre-Change Level
    command(0x3A);
    command(SET_V_VOLTAGE);        //Set vcomH
    command(0x3E);
    command(DEACTIVE_SCROLLING);   //disable scrolling
    command(NORMAL_BRIGHTNESS_DISPLAY_ON);//set display on
}

void SSD1331_clear() {
    int i;
    for(i = 0; i < sizeof(buffer); i++)
    {
        buffer[i] = 0;
    }
}

void SSD1331_draw_point(int x, int y, uint16_t hwColor) {
    if(x >= OLED_WIDTH || y >= OLED_HEIGHT)
    {
        return;
    }
    buffer[x * 2 + y * OLED_WIDTH * 2] = hwColor >> 8;
    buffer[x * 2 + y * OLED_WIDTH * 2 + 1] = hwColor;
}

void SSD1331_char1616(uint8_t x, uint8_t y, uint8_t chChar, uint16_t hwColor) {
    uint8_t i, j;
    uint8_t chTemp = 0, y0 = y;

    for (i = 0; i < 32; i ++) {
        chTemp = Font1612[chChar - 0x30][i];
        for (j = 0; j < 8; j ++) {
            if (chTemp & 0x80) {
                SSD1331_draw_point(x, y, hwColor);
            } else {
                SSD1331_draw_point(x, y, 0);
            }
            chTemp <<= 1;
            y++;
            if ((y - y0) == 16) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

void SSD1331_char3216(uint8_t x, uint8_t y, uint8_t chChar, uint16_t hwColor) {
    uint8_t i, j;
    uint8_t chTemp = 0, y0 = y; 

    for (i = 0; i < 64; i++) {
        chTemp = Font3216[chChar - 0x30][i];
        for (j = 0; j < 8; j++) {
            if (chTemp & 0x80) {
                SSD1331_draw_point(x, y, hwColor);
            } else {
                SSD1331_draw_point(x, y, 0);
            }

            chTemp <<= 1;
            y++;
            if ((y - y0) == 32) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

static void SSD1331_char(uint8_t x, uint8_t y, char acsii, char size, char mode, uint16_t hwColor) {
    uint8_t i, j, y0=y;
    char temp;
    uint8_t ch = acsii - ' ';
    for(i = 0;i<size;i++) {
        if(size == 12)
        {
            if(mode)temp=Font1206[ch][i];
            else temp = ~Font1206[ch][i];
        }
        else 
        {            
            if(mode)temp=Font1608[ch][i];
            else temp = ~Font1608[ch][i];
        }
        for(j =0;j<8;j++)
        {
            if(temp & 0x80) SSD1331_draw_point(x, y, hwColor);
            else SSD1331_draw_point(x, y, 0);
            temp <<=1;
            y++;
            if((y-y0)==size)
            {
                y = y0;
                x ++;
                break;
            }
        }
    }
}

void SSD1331_string(uint8_t x, uint8_t y, const char *pString, uint8_t Size, uint8_t Mode, uint16_t hwColor) {
    while (*pString != '\0') {       
        if (x > (OLED_WIDTH - Size / 2)) {
            x = 0;
            y += Size;
            if (y > (OLED_HEIGHT - Size)) {
                y = x = 0;
            }
        }
        
        SSD1331_char(x, y, *pString, Size, Mode, hwColor);
        x += Size / 2;
        pString ++;
    }
}

void SSD1331_mono_bitmap(uint8_t x, uint8_t y, const uint8_t *pBmp, char chWidth, char chHeight, uint16_t hwColor) {
    uint8_t i, j, byteWidth = (chWidth + 7) / 8;
    for(j = 0; j < chHeight; j++) {
        for(i = 0; i <chWidth; i ++) {
            if(*(pBmp + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                SSD1331_draw_point(x + i, y + j, hwColor);
            }
        }
    }        
}

void SSD1331_bitmap24(uint8_t x, uint8_t y, uint8_t *pBmp, char chWidth, char chHeight) {
    uint8_t i, j;
    uint16_t hwColor;
    uint32_t temp;

    for(j = 0; j < chHeight; j++) {
        for(i = 0; i < chWidth; i ++) {
            temp = *(unsigned int*)(pBmp + i * 3 + j * 3 * chWidth);
            hwColor = RGB(((temp >> 16) & 0xFF),
                          ((temp >> 8) & 0xFF),
                           (temp & 0xFF));
            SSD1331_draw_point(x + i, y + chHeight - 1 - j, hwColor);
        }
    }
}

void SSD1331_display() {
    command(SET_COLUMN_ADDRESS);
    command(0);         //cloumn start address
    command(OLED_WIDTH - 1); //cloumn end address
    command(SET_ROW_ADDRESS);
    command(0);         //page atart address
    command(OLED_HEIGHT - 1); //page end address
    bcm2835_gpio_write(DC, HIGH);
    bcm2835_spi_transfern(buffer, sizeof(buffer));
}

void SSD1331_clear_screen(uint16_t hwColor) {
    uint16_t i, j;
    for(i = 0; i < OLED_HEIGHT; i++) {
        for(j = 0; j < OLED_WIDTH; j ++) {
            SSD1331_draw_point(j, i, hwColor);
        }
    }
}

