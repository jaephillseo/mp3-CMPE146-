/*
 * MP3Driver.hpp
 *
 *  Created on: Aug 5, 2018
 *      Author: Khoa
 */

#ifndef MP3DRIVER_HPP_
#define MP3DRIVER_HPP_

#include "stdint.h"
#include "LabGPIO.hpp"

#define SCI_WRITE_OPCODE 0x02
#define SCI_CLK_REG 0x03
#define SCI_VOL_REG 0x0B

class MP3Driver
{
private:
    uint8_t SCI_VOL_LEFT = 0x00; // 0x00 (max) to 0xFE (min) = 0 to 254 = 0 to -127dB
    uint8_t SCI_VOL_RIGHT = 0x00;
public:

    void init();

    void waitForDREQ();

    void writeToSCIReg(char addr, char msb, char lsb);

    void sendSDIByte(uint8_t byte);

    void setXDCSLow();

    void setXDCSHigh();

    bool requestPause();

    bool requestBack();

    bool requestNext();

    bool requestVolDown();

    bool requestVolUp();

    void decreaseVol();

    void increaseVol();

    bool selectUp();

    bool selectdown();
};
#endif /* MP3DRIVER_HPP_ */
