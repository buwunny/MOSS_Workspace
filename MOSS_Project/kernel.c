// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  1.0
//
//       FILE NAME:  kernel.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains a collection of functions for initializing the basic
//    kernel created for the CSC202 final project.
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    This code was developed for educational purposes as part of the CSC202 
//    course and is provided "as is" without warranties of any kind, whether 
//    express, implied, or statutory.
//
//    The author and organization do not warrant the accuracy, completeness, or
//    reliability of the code. The author and organization shall not be liable
//    for any direct, indirect, incidental, special, exemplary, or consequential
//    damages arising out of the use of or inability to use the code, even if
//    advised of the possibility of such damages.
//
//    Use of this code is at your own risk, and it is recommended to validate
//    and adapt the code for your specific application and hardware requirements.
//
// Copyright (c) 2024 by TBD
//    You may use, edit, run or distribute this file as long as the above
//    copyright notice remains
// *****************************************************************************
//******************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>


//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include <ti/devices/msp/m0p/mspm0g350x.h>
#include "kernel.h"
#include "clock.h"
#include "lcd1602.h"
#include "LaunchPad.h"
#include "rtc.h"
#include "adc.h"
#include "spi.h"
#include "ili9341.h"


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function initializes the basic kernel components. It sets up the clock,
//  GPIO, I2C, LCD, ADC, LEDs, RTC, SysTick, SPI, and ILI9341 components.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//------------------------------------------------------------------------------
void kernel_init(void)
{
  clock_init_80mhz();
  // clock_init_40mhz();
  launchpad_gpio_init();
  I2C_init();
  lcd1602_init();
  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
  lp_leds_init();
  RTC_init();
  sys_tick_init(SYST_TICK_PERIOD_COUNT);
  spi1_init_40mhz();
  ili9341_init();
  ili9341_fill_screen(ILI9341_WHITE);
} /* kernel_init */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function attempts to measure the clock frequency by using the SysTick
//  timer and a known time delay.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  The measured amount of ticks that occurred during the 100ms delay. Should be
//  close to 8000000 for an 80MHz clock and 4000000 for a 40MHz clock.
//------------------------------------------------------------------------------
uint32_t measure_clock(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 0xFFFFFF;  // max load
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;

    msec_delay(100);

    uint32_t elapsed = 0xFFFFFF - SysTick->VAL;
    SysTick->CTRL = 0;

    return elapsed;
}