/*
 * MP3Driver.cpp
 *
 *  Created on: Aug 5, 2018
 *      Author: Khoa
 */

#include "MP3Driver.hpp"
#include "ssp0.h"
#include "LabGPIO.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

LabGPIO XDCS(1, 19), CS(1, 20), RST(1, 22), DREQ(1, 23),
        pauseButton(2, 1),  backButton(2, 2), nextButton(2, 3), volDownButton(2, 4), volUpButton(2, 5),
        selectUpButton(2, 6), selectDownButton(2, 7);

void MP3Driver::init()
{
    ssp0_init(8);

    XDCS.setAsOutput();
    CS.setAsOutput();
    RST.setAsOutput();
    DREQ.setAsInput();

    pauseButton.setAsInput();
    backButton.setAsInput();
    nextButton.setAsInput();
    volDownButton.setAsInput();
    volUpButton.setAsInput();
    selectUpButton.setAsInput();
    selectDownButton.setAsInput();

    RST.setLow();
    RST.setHigh();
    XDCS.setHigh();
    CS.setHigh();

    writeToSCIReg(SCI_CLK_REG, 0x20, 0x00); //External clock multiplier (x2)
    printf("\nWriteSCI complete!\n");
}

void MP3Driver::waitForDREQ()
{
    while (!DREQ.getLevel())
    {
        vTaskDelay(1);
    }
}

void MP3Driver::writeToSCIReg(char addr, char msb, char lsb)
{
    waitForDREQ();
    CS.setLow();
    ssp0_exchange_byte(SCI_WRITE_OPCODE);
    ssp0_exchange_byte(addr);
    ssp0_exchange_byte(msb);
    ssp0_exchange_byte(lsb);
    CS.setHigh();
}

void MP3Driver::sendSDIByte(uint8_t byte)
{
    ssp0_exchange_byte(byte);
}

void MP3Driver::setXDCSLow()
{
    XDCS.setLow();
}

void MP3Driver::setXDCSHigh()
{
    XDCS.setHigh();
}

bool MP3Driver::requestPause()
{
    if (pauseButton.getLevel())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MP3Driver::requestBack()
{
    if (backButton.getLevel())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MP3Driver::requestNext()
{
    if (nextButton.getLevel())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MP3Driver::requestVolDown()
{
    if (volDownButton.getLevel())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MP3Driver::requestVolUp()
{
    if (volUpButton.getLevel())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MP3Driver::decreaseVol()
{
    if (SCI_VOL_LEFT <= 0xF7) // 0xF7 + 0x07 = 0xFE (min volume)
    {
        SCI_VOL_LEFT += 0x07;
        SCI_VOL_RIGHT += 0x07;
        writeToSCIReg(SCI_VOL_REG, SCI_VOL_LEFT, SCI_VOL_RIGHT);
    }
}

void MP3Driver::increaseVol()
{
    if (SCI_VOL_LEFT >= 0x07) // 0x07 - 0x07 = 0x00 (max volume)
    {
        SCI_VOL_LEFT -= 0x07;
        SCI_VOL_RIGHT -= 0x07;
        writeToSCIReg(SCI_VOL_REG, SCI_VOL_LEFT, SCI_VOL_RIGHT);
    }
}

bool MP3Driver::selectUp()
{
    if (selectUpButton.getLevel())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MP3Driver::selectdown()
{
    if (selectDownButton.getLevel())
    {
        return true;
    }
    else
    {
        return false;
    }
}
