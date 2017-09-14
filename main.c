/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
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

#include "mcc_generated_files/mcc.h"
#include "CW201x.h"
extern STRUCT_CW_BATTERY   cw_bat;
unsigned char count = 0;
/*
                         Main application
 */

void main(void)
{
    // initialize the device
    unsigned char ret = 0;
    uint8_t send_char;
    SYSTEM_Initialize();
    IO_RC2_SetHigh();
    IO_RA2_SetHigh();
    IO_RA5_SetHigh();
    IO_RA4_SetHigh();
    ret = cw_bat_init();
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    cw_bat_work();

    while (1)
    {
        // Add your application code
        if(cw_bat.capacity >= 80)
        {
            IO_RC2_SetLow();
            IO_RA2_SetLow();
            IO_RA5_SetLow();
            IO_RA4_SetLow();
        }
        else if(cw_bat.capacity >= 60)
        {
            IO_RC2_SetHigh();
            IO_RA2_SetLow();
            IO_RA5_SetLow();
            IO_RA4_SetLow();
        }
        else if(cw_bat.capacity >= 40)
        {
            IO_RC2_SetHigh();
            IO_RA2_SetHigh();
            IO_RA5_SetLow();
            IO_RA4_SetLow();
        }
        else if(cw_bat.capacity >= 20)
        {
            IO_RC2_SetHigh();
            IO_RA2_SetHigh();
            IO_RA5_SetHigh();
            IO_RA4_SetLow();
        }
        else
        {
            IO_RC2_SetHigh();
            IO_RA2_SetHigh();
            IO_RA5_SetHigh();
            IO_RA4_SetHigh();
        }
        
        if(count < 10)
        {
            CW_Delay10ms(333);
            count ++;
        }
        else
        {
            CW_Delay10ms(1000);
            CW_Delay10ms(1000);
            CW_Delay10ms(1000);
            CW_Delay10ms(1000);
        }
        
        cw_bat_work();
        
        EUSART1_Write('S');
        EUSART1_Write(cw_bat.voltage);
        EUSART1_Write(cw_bat.capacity);
        EUSART1_Write('E');
    }
}
/**
 End of File
*/ 