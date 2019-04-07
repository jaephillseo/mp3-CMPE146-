/*
 * LabGPIO_0.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: Khoa
 */

#include "lpc17xx.h"
#include "LabGPIO_0.hpp"

    /**
     * You should not modify any hardware registers at this point
     * You should store the port and pin using the constructor.
     *
     * @param {uint8_t} pin  - pin number between 0 and 32
     */
    LabGPIO_0::LabGPIO_0(uint8_t pin)
    {
        pinNum = pin;
    }
    /**
     * Should alter the hardware registers to set the pin as an input
     */
    void LabGPIO_0::setAsInput()
    {
        LPC_GPIO0->FIODIR &= ~(1 << pinNum);
    }
    /**
     * Should alter the hardware registers to set the pin as an output
     */
    void LabGPIO_0::setAsOutput()
    {
        LPC_GPIO0->FIODIR |= (1 << pinNum);
    }
    /**
     * Should alter the set the direction output or input depending on the input.
     *
     * @param {bool} output - true => output, false => set pin to input
     */
    void LabGPIO_0::setDirection(bool output)
    {
        if (output)
        {
            LPC_GPIO0->FIODIR |= (1 << pinNum);
        }
        else
        {
            LPC_GPIO0->FIODIR &= ~(1 << pinNum);
        }
    }
    /**
     * Should alter the hardware registers to set the pin as high
     */
    void LabGPIO_0::setHigh()
    {
        LPC_GPIO0->FIOSET = (1 << pinNum);
    }
    /**
     * Should alter the hardware registers to set the pin as low
     */
    void LabGPIO_0::setLow()
    {
        LPC_GPIO0->FIOCLR = (1 << pinNum);
    }
    /**
     * Should alter the hardware registers to set the pin as low
     *
     * @param {bool} high - true => pin high, false => pin low
     */
    void LabGPIO_0::set(bool high)
    {
        if (high)
        {
            LPC_GPIO0->FIOSET = (1 << pinNum);
        }
        else
        {
            LPC_GPIO0->FIOCLR = (1 << pinNum);
        }
    }
    /**
     * Should return the state of the pin (input or output, doesn't matter)
     *
     * @return {bool} level of pin high => true, low => false
     */
    bool LabGPIO_0::getLevel()
    {
        if (LPC_GPIO0->FIOPIN & (1 << pinNum))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
