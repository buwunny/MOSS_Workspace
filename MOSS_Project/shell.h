// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  0.1
//
//       FILE NAME:  shell.h
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

#ifndef __SHELL_H__
#define __SHELL_H__

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define SHELL_MAX_INPUT_LENGTH                                              (16)
#define CARRIAGE_RETURN_CHAR                                              ('\r')
#define NULL_CHAR                                                         ('\0')
#define BACKSPACE_CHAR                                                    ('\b')
#define NEWLINE_CHAR                                                      ('\n')
#define DEGREE_SYMBOL                                               (0b11011111)
#define TEMP_SENSOR_CHANNEL                                                  (5)
#define CONVERT_TO_FAHRENHEIT(x)                              ((x) * 9 / 5 + 32)


// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void shell_init(void);
void shell_loop(void);
void shell_handle_input(char* input);
void UART_interrupt_enable(void);
void UART_write_string(char* string);
void lcd_write_time(uint8_t hour, uint8_t min, uint8_t sec);
void show_temp(void);
void UART0_IRQHandler(void);
void RTC_IRQHandler(void);
uint32_t measure_clock(void);

#endif /* __SHELL_H__ */