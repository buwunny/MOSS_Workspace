// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  0.1
//
//       FILE NAME:  isr.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains the ISRs.
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
//    and adapt the code for your specific application and hardware 
//    requirements.
//
// Copyright (c) 2024 by TBD
//    You may use, edit, run or distribute this file as long as the above
//    copyright notice remains
// *****************************************************************************
//******************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "isr.h"
#include "LaunchPad.h"
#include "lcd1602.h"
#include "adc.h"


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function represents the ISR (Interrupt Service Routine) for the SysTick
//  timer. It is called at regular intervals based on the configured SysTick
//  period.
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
void SysTick_Handler(void)
{
  static bool on = false;
  
  if (!on)
  {
    lp_leds_on(LP_RED_LED1_IDX);
  } /* if */
  else
  {
    lp_leds_off(LP_RED_LED1_IDX);
  } /* else */

  on = !on;
} /* SysTick_Handler */


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function represents the ISR (Interrupt Service Routine) for the RTC.
//  Every second the LCD time display updates and every 10 seconds the LCD
//  temperature display updates.
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
void RTC_IRQHandler(void)
{
  uint32_t iidx = RTC->CPU_INT.IIDX;   // Read (clears)
  switch (iidx)
  {
    case RTC_CPU_INT_IIDX_STAT_RTCRDY:
      RTC->CPU_INT.ICLR = RTC_CPU_INT_ICLR_RTCRDY_CLR;
      if (RTC->SEC % 10 == 0)
      {
        uint16_t adc_temp_result = ADC0_in(TEMP_SENSOR_CHANNEL);
        uint8_t temperature_c = thermistor_calc_temperature(adc_temp_result);
        uint8_t temperature_f = CONVERT_TO_FAHRENHEIT(temperature_c);

        lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_12);
        lcd_write_temp(temperature_f);
      } /* if */
      lcd_set_ddram_addr(LCD_LINE1_ADDR);
      lcd_write_time(RTC->HOUR, RTC->MIN, RTC->SEC);
      break;
    default:
      break;
  } /* switch */
} /* RTC_IRQHandler */