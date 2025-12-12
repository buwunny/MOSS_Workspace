// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  1.0
//
//       FILE NAME:  shell.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains the implementation of a simple shell that can be used
//    to interact with a microcontroller. The shell supports basic commands 
//    such as displaying help, measuring clock speed, reading temperature from
//    a thermistor, displaying the current RTC time, running LCD color test,
//    and clearing the terminal. The shell uses UART for communication and 
//    displays output to both the UART and an LCD display.
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


#include "ili9341.h"


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define SHELL_MAX_INPUT_LENGTH                                             (128)
#define SHELL_LINE_HEIGHT                                                   (25)
#define SHELL_CHAR_PER_LINE                     (ILI9341_TFTWIDTH / GLYPH_WIDTH)
#define CARRIAGE_RETURN_CHAR                                              ('\r')
#define NULL_CHAR                                                         ('\0')
#define BACKSPACE_CHAR                                                    ('\b')
#define NEWLINE_CHAR                                                      ('\n')
#define BAUD_RATE                                                       (115200)
#define TEMP_SENSOR_CHANNEL                                                  (5)
#define CONVERT_TO_FAHRENHEIT(x)                              ((x) * 9 / 5 + 32)

// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void shell_init(void);
void shell_loop(void);
void shell_handle_input(char* input);
void shell_draw_char(char c);
void shell_erase_char(char c);
void shell_draw_string(char* str);
void shell_new_line(void);

#endif /* __SHELL_H__ */