/**
  ******************************************************************************
    @file    epd.h
    @author  Waveshare Team
    @version V1.0.0
    @date    23-January-2018
    @brief   This file provides e-Paper driver functions
              void EPD_SendCommand(byte command);
              void EPD_SendData(byte data);
              void EPD_WaitUntilIdle();
              void EPD_Send_1(byte c, byte v1);
              void EPD_Send_2(byte c, byte v1, byte v2);
              void EPD_Send_3(byte c, byte v1, byte v2, byte v3);
              void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4);
              void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5);
              void EPD_Reset();
              void EPD_dispInit();

             varualbes:
              EPD_dispLoad;                - pointer on current loading function
              EPD_dispIndex;               - index of current e-Paper
              EPD_dispInfo EPD_dispMass[]; - array of e-Paper properties

  ******************************************************************************
*/

#include <SPI.h>

/* SPI pin definition --------------------------------------------------------*/
// SPI pin definition
#define CS_PIN 15
#define RST_PIN 2
#define DC_PIN 4
#define BUSY_PIN 5

/* Pin level definition ------------------------------------------------------*/
#define LOW 0
#define HIGH 1

#define GPIO_PIN_SET 1
#define GPIO_PIN_RESET 0

/* Lut mono ------------------------------------------------------------------*/
byte lut_full_mono[] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00};

byte lut_partial_mono[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* The procedure of sending a byte to e-Paper by SPI -------------------------*/
void EpdSpiTransferCallback(byte data)
{
    digitalWrite(CS_PIN, GPIO_PIN_RESET);
    SPI.transfer(data);
    digitalWrite(CS_PIN, GPIO_PIN_SET);
}

byte lut_vcom0[] = {15, 0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00};
byte lut_w[] = {15, 0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04};
byte lut_b[] = {15, 0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04};
byte lut_g1[] = {15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04};
byte lut_g2[] = {15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04};
byte lut_vcom1[] = {15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte lut_red0[] = {15, 0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte lut_red1[] = {15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Sending a byte as a command -----------------------------------------------*/
void EPD_SendCommand(byte command)
{
    digitalWrite(DC_PIN, LOW);
    EpdSpiTransferCallback(command);
}

/* Sending a byte as a data --------------------------------------------------*/
void EPD_SendData(byte data)
{
    digitalWrite(DC_PIN, HIGH);
    EpdSpiTransferCallback(data);
}

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle()
{
    //0: busy, 1: idle
    while (digitalRead(BUSY_PIN) == 0)
        delay(1000);
}

/* Send a one-argument command -----------------------------------------------*/
void EPD_Send_1(byte c, byte v1)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
}

/* Send a two-arguments command ----------------------------------------------*/
void EPD_Send_2(byte c, byte v1, byte v2)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
}

/* Send a three-arguments command --------------------------------------------*/
void EPD_Send_3(byte c, byte v1, byte v2, byte v3)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
}

/* Send a four-arguments command ---------------------------------------------*/
void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
    EPD_SendData(v4);
}

/* Writting lut-data into the e-Paper ----------------------------------------*/
void EPD_lut(byte c, byte l, byte *p)
{
    // lut-data writting initialization
    EPD_SendCommand(c);

    // lut-data writting doing
    for (int i = 0; i < l; i++, p++)
        EPD_SendData(*p);
}

/* This function is used to 'wake up" the e-Paper from the deep sleep mode ---*/
void EPD_Reset()
{
    digitalWrite(RST_PIN, LOW);
    delay(200);

    digitalWrite(RST_PIN, HIGH);
    delay(200);
}

/* e-Paper initialization functions ------------------------------------------*/
#include "epd4in2.h"

int EPD_dispIndex;        // The index of the e-Paper's type

/* Image data loading function for a-type e-Paper ----------------------------*/
void EPD_loadA(byte value)
{
    EPD_SendData(value);
}

/* Show image and turn to deep sleep mode (b-type, e-Paper) ------------------*/
void EPD_showB()
{
    // Refresh
    EPD_SendCommand(0x12); //DISPLAY_REFRESH
    delay(100);
    EPD_WaitUntilIdle();

    // Sleep
    EPD_Send_1(0x50, 0x17);                   //VCOM_AND_DATA_INTERVAL_SETTING
    EPD_Send_1(0x82, 0x00);                   //VCM_DC_SETTING_REGISTER, to solve Vcom drop
    EPD_Send_4(0x01, 0x02, 0x00, 0x00, 0x00); //POWER_SETTING
    EPD_WaitUntilIdle();
    EPD_SendCommand(0x02); //POWER_OFF
}
