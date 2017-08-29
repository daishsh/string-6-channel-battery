/**
  EUSART1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    eusart1.c

  @Summary
    This is the generated driver implementation file for the EUSART1 driver using MPLAB(c) Code Configurator

  @Description
    This header file provides implementations for driver APIs for EUSART1.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.15.1
        Device            :  PIC16F15323
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "eusart1.h"
#include "pin_manager.h"
#include "../CW201x.h"

/**
  Section: EUSART1 APIs
*/
#define ID_HIGH 0xAA
#define ID_LOW 0xAA

unsigned char rxbuf[15];
unsigned char count = 0;
unsigned char state = 0;
extern STRUCT_CW_BATTERY   cw_bat;

void EUSART1_Initialize(void)
{
    // disable interrupts before changing states
    PIE3bits.RC1IE = 0;
    PIE3bits.TX1IE = 0;

    // Set the EUSART1 module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUD1CON = 0x08;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TX1STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 64; 
    SP1BRGL = 0x40;

    // Baud Rate = 9600; SP1BRGH 3; 
    SP1BRGH = 0x03;


    // initializing the driver state

    // enable receive interrupt
    PIE3bits.RC1IE = 1;
}

void EUSART1_Write(uint8_t txData)
{
    while(0 == PIR3bits.TX1IF)
    {
    }

    TX1REG = txData;    // Write the data byte to the USART.
}

void EUSART1_Receive_ISR(void)
{
    unsigned char dat;
    if(1 == RC1STAbits.OERR)
    {
        // EUSART1 error - restart

        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
    }

    // buffer overruns are ignored
    dat = RC1REG;
    
    if((dat == 'S') && (count == 0))
    {
        count = 1;
    }
    else if(dat == 'E')
    {
        count ++;
        if((count == 4)  && (rxbuf[0] == ID_HIGH) && (rxbuf[1] == ID_LOW))
        {
            IO_RC3_SetHigh();
            CW_Delay10ms(1);
            EUSART1_Write('S');
            EUSART1_Write(ID_HIGH);
            EUSART1_Write(ID_LOW);
            EUSART1_Write(cw_bat.voltage);
            EUSART1_Write(cw_bat.capacity);
            EUSART1_Write('E');
            CW_Delay10ms(1);
            IO_RC3_SetLow();
        }
        count = 0;
    }
    else if(count > 0)
    {
        rxbuf[count - 1] = dat;
        count ++;
        if(count > 15) count = 15;
    }
    else ;
}
/**
  End of File
*/
