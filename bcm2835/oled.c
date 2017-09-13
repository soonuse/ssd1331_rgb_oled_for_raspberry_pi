/*****************************************************************************
*
* File                : oled.c
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
#include <time.h>
#include "ssd1331.h"

char value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
int main(int argc, char **argv)
{
    time_t now;
    struct tm *timenow;
    FILE *pFile = fopen("bitmap24.bmp", "r");
    /* 1 pixel of 888 bitmap = 3 bytes */
    size_t pixelSize = 3;
    unsigned char bmpBuffer[OLED_WIDTH * OLED_HEIGHT * 3];

    if(!bcm2835_init())
    {
        return -1;
    }
    if (pFile == NULL) {
        printf("file not exist\n");
        return 0;
    }
    fseek(pFile, 54, 0);
    fread(bmpBuffer, pixelSize, OLED_WIDTH * OLED_HEIGHT, pFile);
    fclose(pFile);

    printf("OLED example. Press Ctrl + C to exit.\n");
    SSD1331_begin();
    SSD1331_mono_bitmap(0, 0, waveshare_logo, 96, 64, BLUE);
    SSD1331_display();
    bcm2835_delay(2000);
    SSD1331_bitmap24(0, 0, bmpBuffer, 96, 64);
    SSD1331_display();
    bcm2835_delay(2000);

    SSD1331_clear();
    while(1)
    {
        time(&now);
        timenow = localtime(&now);

        SSD1331_mono_bitmap(0, 2, Signal816, 16, 8, GOLDEN); 
        SSD1331_mono_bitmap(19, 2, Msg816, 16, 8, GOLDEN); 
        SSD1331_mono_bitmap(38, 2, Bluetooth88, 8, 8, GOLDEN); 
        SSD1331_mono_bitmap(52, 2, GPRS88, 8, 8, GOLDEN); 
        SSD1331_mono_bitmap(66, 2, Alarm88, 8, 8, GOLDEN); 
        SSD1331_mono_bitmap(80, 2, Bat816, 16, 8, GOLDEN); 

        SSD1331_string(0, 52, "MUSIC", 12, 0, WHITE); 
        SSD1331_string(64, 52, "MENU", 12, 1, WHITE); 

        SSD1331_char3216(0, 16, value[timenow->tm_hour / 10], BLUE);
        SSD1331_char3216(16, 16, value[timenow->tm_hour % 10], BLUE);
        SSD1331_char3216(40, 16, ':', RED);
        SSD1331_char3216(64, 16, value[timenow->tm_min / 10], GREEN);
        SSD1331_char3216(80, 16, value[timenow->tm_min % 10], GREEN);
        
        SSD1331_display();
    }
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

