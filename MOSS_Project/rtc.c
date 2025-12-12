// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  1.0
//
//       FILE NAME:  rtc.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains a function for initializing the RTC.
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


//------------------------------------------------------------------------------
// DESCRIPTION:
//  This function initializes the RTC module and enables its interrupts.
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
void RTC_init(void)
{ 
  // Enable power to RTC
  RTC->GPRCM.PWREN = (RTC_PWREN_KEY_UNLOCK_W | RTC_PWREN_ENABLE_ENABLE);

  // Set to binary mode
  RTC->CTL |= RTC_CTL_RTCBCD_BINARY;

  // Enable the RTC
  RTC->CLKCTL = RTC_CLKCTL_MODCLKEN_ENABLE;
  
  // Ensure bit is cleared
  RTC->CPU_INT.ICLR = RTC_CPU_INT_ICLR_RTCRDY_CLR;
  
  // Unmask RTC interrupt
  RTC->CPU_INT.IMASK = RTC_CPU_INT_IMASK_RTCRDY_SET;
  
  // Enable interrupt
  NVIC_EnableIRQ(RTC_INT_IRQn);
} /* rtc_init */