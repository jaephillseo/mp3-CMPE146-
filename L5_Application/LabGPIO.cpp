/*
 * LabGPIO.cpp
 *
 *  Created on: Jun 17, 2018
 *      Author: Khoa
 */

#include "lpc17xx.h"
#include "LabGPIO.hpp"

LabGPIO::LabGPIO(uint8_t port, uint8_t pin)
{
    portNum = port;
    pinNum = pin;
}

void LabGPIO::setAsInput()
{
    if (portNum == 0)
    {
        LPC_GPIO0->FIODIR &= ~(1 << pinNum);
    }
    else if (portNum == 1)
    {
        LPC_GPIO1->FIODIR &= ~(1 << pinNum);
    }
    else if (portNum == 2)
    {
        LPC_GPIO2->FIODIR &= ~(1 << pinNum);
    }
}

void LabGPIO::setAsOutput()
{
    if (portNum == 0)
    {
        LPC_GPIO0->FIODIR |= (1 << pinNum);
    }
    else if (portNum == 1)
    {
        LPC_GPIO1->FIODIR |= (1 << pinNum);
    }
    else if (portNum == 2)
    {
        LPC_GPIO2->FIODIR |= (1 << pinNum);
    }
}

void LabGPIO::setDirection(bool output)
{
    if (output)
    {
        if (portNum == 0)
        {
            LPC_GPIO0->FIODIR |= (1 << pinNum);
        }
        else if (portNum == 1)
        {
            LPC_GPIO1->FIODIR |= (1 << pinNum);
        }
        else if (portNum == 2)
        {
            LPC_GPIO2->FIODIR |= (1 << pinNum);
        }
    }
    else
    {
        if (portNum == 0)
        {
            LPC_GPIO0->FIODIR &= ~(1 << pinNum);
        }
        else if (portNum == 1)
        {
            LPC_GPIO1->FIODIR &= ~(1 << pinNum);
        }
        else if (portNum == 2)
        {
            LPC_GPIO2->FIODIR &= ~(1 << pinNum);
        }
    }
}

void LabGPIO::setHigh()
{
    if (portNum == 0)
    {
        LPC_GPIO0->FIOSET = (1 << pinNum);
    }
    else if (portNum == 1)
    {
        LPC_GPIO1->FIOSET = (1 << pinNum);
    }
    else if (portNum == 2)
    {
        LPC_GPIO2->FIOSET = (1 << pinNum);
    }
}

void LabGPIO::setLow()
{
    if (portNum == 0)
    {
        LPC_GPIO0->FIOCLR = (1 << pinNum);
    }
    else if (portNum == 1)
    {
        LPC_GPIO1->FIOCLR = (1 << pinNum);
    }
    else if (portNum == 2)
    {
        LPC_GPIO2->FIOCLR = (1 << pinNum);
    }
}

void LabGPIO::set(bool high)
{
    if (high)
    {
        if (portNum == 0)
        {
            LPC_GPIO0->FIOSET = (1 << pinNum);
        }
        else if (portNum == 1)
        {
            LPC_GPIO1->FIOSET = (1 << pinNum);
        }
        else if (portNum == 2)
        {
            LPC_GPIO2->FIOSET = (1 << pinNum);
        }
    }
    else
    {
        if (portNum == 0)
        {
            LPC_GPIO0->FIOCLR = (1 << pinNum);
        }
        else if (portNum == 1)
        {
            LPC_GPIO1->FIOCLR = (1 << pinNum);
        }
        else if (portNum == 2)
        {
            LPC_GPIO2->FIOCLR = (1 << pinNum);
        }
    }
}

bool LabGPIO::getLevel()
{
    if (portNum == 0)
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
    else if (portNum == 1)
    {
        if (LPC_GPIO1->FIOPIN & (1 << pinNum))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (portNum == 2)
    {
        if (LPC_GPIO2->FIOPIN & (1 << pinNum))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
