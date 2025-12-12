// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  1.0
//
//       FILE NAME:  kernel.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
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

#ifndef __KERNEL_H__
#define __KERNEL_H__

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define MSPM0_CLOCK_FREQUENCY                                             (80E6)
#define SYST_TICK_PERIOD                                                   (1E3)
#define SYST_TICK_PERIOD_COUNT        (SYST_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)


// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void kernel_init(void);
uint32_t measure_clock(void);

#endif /* __KERNEL_H__ */