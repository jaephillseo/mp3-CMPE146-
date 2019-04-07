/*
 * LABUART.h
 *
 *  Created on: Mar 15, 2018
 *      Author: God
 */

#ifndef LABUART_H_
#define LABUART_H_

#include <stdint.h>

#include "LPC17xx.h"

class LabUART
{
private:
    LPC_UART_TypeDef* const UART[2] = {LPC_UART2, LPC_UART3};

public:
    enum Peripheral
    {
        UART2 = 0,
        UART3 = 1,
    };

    enum WordLength
    {
       five_bit = 0,
       six_bit = 1,
       seven_bit = 2,
       eight_bit = 3,
    };

    // TODO: Fill in methods for init(), transmit(), receive() etc.
    bool init(Peripheral peripheral, uint16_t baud_rate, WordLength bit_length, void (*uart_isr)(void));
    void transmit(Peripheral peripheral, uint8_t send);
    uint8_t receive(Peripheral peripheral);

    LabUART();
    ~LabUART();
};


#endif /* LABUART_H_ */
