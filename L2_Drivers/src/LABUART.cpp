/*
 * LABUART.cpp
 *
 *  Created on: Mar 15, 2018
 *      Author: God
 */
#include <stdint.h>

#include "LABUART.h"
#include "LPC17xx.h"
#include "sys_config.h"
#include "FreeRTOS.h"


bool LabUART::init(Peripheral peripheral, uint16_t baud_rate, WordLength bit_length, void (*uart_isr)(void))
{
    bool successful = true;

    if((peripheral != UART2) && (peripheral != UART3))
    {
        successful = false;
    }
    if((bit_length != five_bit) && (bit_length != six_bit) && (bit_length != seven_bit) && (bit_length != eight_bit))
    {
        successful = false;
    }

    if(peripheral == UART2)
    {
        /* Set PCONP UART2 */
        LPC_SC->PCONP |= (1 << 24);

        /* Set Peripheral Clock */
        LPC_SC->PCLKSEL1 &= ~(3 << 16);
        LPC_SC->PCLKSEL1 |= (1 << 16);

        /* Select Pins */
        LPC_PINCON->PINSEL4 &= ~(3 << 16);
        LPC_PINCON->PINSEL4 |= (2 << 16);
        LPC_PINCON->PINSEL4 &= ~(3 << 18);
        LPC_PINCON->PINSEL4 |= (2 << 18);

        isr_register(UART2_IRQn, uart_isr);
        NVIC_EnableIRQ(UART2_IRQn);
    }
    else
    {
        /* Set PCONP UART3 */
        LPC_SC->PCONP |= (1 << 25);

        /* Set Peripheral Clock */
        LPC_SC->PCLKSEL1 &= ~(3 << 18);
        LPC_SC->PCLKSEL1 |= (1 << 18);

        /* Select Pins */
        LPC_PINCON->PINSEL9 |= (3 << 24);
        LPC_PINCON->PINSEL9 |= (3 << 26);

        isr_register(UART3_IRQn, uart_isr);
        NVIC_EnableIRQ(UART3_IRQn);
    }

    /* Set Baud Rate */
    UART[peripheral]->LCR |= (1 << 7);
    UART[peripheral]->DLM &= ~(0xFFFFFFFF);
    UART[peripheral]->DLL &= ~(0xFFFFFFFF);
    UART[peripheral]->DLM |= ((sys_get_cpu_clock() / (16 * baud_rate)) >> 8);
    UART[peripheral]->DLL |= (sys_get_cpu_clock() / (16 * baud_rate));
    UART[peripheral]->LCR &= ~(1 << 7);

    /* Set Frame */
    UART[peripheral]->LCR &= ~(3 << 0);
    UART[peripheral]->LCR |= bit_length;

    // Init UART Rx interrupt (TX interrupt is optional)
    UART[peripheral]->IER |= 1;

    return successful;
}

void LabUART::transmit(Peripheral peripheral, uint8_t send)
{
    UART[peripheral]->THR = send;
    while(! (UART[peripheral]->LSR & (1 << 6)));
}

uint8_t LabUART::receive(Peripheral peripheral)
{
    while(! (UART[peripheral]->LSR & 1));
    return (UART[peripheral]->RBR & 0xFF);
}

LabUART::LabUART()
{

}

LabUART::~LabUART()
{

}
