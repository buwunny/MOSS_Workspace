// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Rafael Ortiz
//
//         VERSION:  0.1
//
//       FILE NAME:  shell.c
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

#include <ti/devices/msp/msp.h>
#include <ti/devices/msp/m0p/mspm0g350x.h>
#include <string.h>
#include "shell.h"
#include "uart.h"
#include "kernel.h"
#include "lcd1602.h"
#include "clock.h"
#include "adc.h"
#include "ili9341.h"


void shell_init(void)
{
  UART_init(115200);
  UART_write_string("\nWelcome back!\n");
  
} /* shell_init */

void shell_loop(void)
{
  while (1)
  {
    char buffer[SHELL_MAX_INPUT_LENGTH];
    uint8_t index = 0;
    char input;
    do
    {      
      input = UART_in_char();
      if (input == CARRIAGE_RETURN_CHAR)
      {
        UART_out_char(input);
        UART_out_char(NEWLINE_CHAR);
        buffer[index] = NULL_CHAR;
      } /* if */
      else if (input == BACKSPACE_CHAR) {
        UART_out_char(input);
        if (index > 0)
        {
          index--;
          lcd_set_ddram_addr(LCD_LINE2_ADDR + index);
          lcd_write_char(' ');
          lcd_set_ddram_addr(LCD_LINE2_ADDR + index);
        } /* if */
      } /* else if */
      else {
        if (index < SHELL_MAX_INPUT_LENGTH)
        {
          UART_out_char(input);
          buffer[index] = input;
          index++;
          lcd_set_ddram_addr(LCD_LINE2_ADDR + index - 1);
          lcd_write_char(input);
        }
      } /* else */
    } while (input != CARRIAGE_RETURN_CHAR);
    shell_handle_input(buffer);
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("                ");
  }
}

void shell_handle_input(char* input)
{
  UART_write_string("\r\n");
  if (strcmp(input, "help") == 0)
  {
    UART_write_string("Available commands:\r\n");
    UART_write_string("  help  - Show this help message\r\n");
    UART_write_string("  clock - Measure clock speed\r\n");
    UART_write_string("  temp  - Read temperature from thermistor\r\n");
    UART_write_string("  time  - Display current RTC time\r\n");
    UART_write_string("  color - Run LCD color test\r\n");
  }
  else if (strcmp(input, "clock") == 0)
  {
    uint32_t clock_measurement = measure_clock();
    char output_buffer[50];
    sprintf(output_buffer, "Clock cycles in 100ms: %u\r\n", clock_measurement);
    UART_write_string(output_buffer);
  }
  else if (strcmp(input, "temp") == 0)
  {
    uint16_t adc_temp_result = ADC0_in(TEMP_SENSOR_CHANNEL);
    uint8_t temperature_c = thermistor_calc_temperature(adc_temp_result);
    uint8_t temperature_f = CONVERT_TO_FAHRENHEIT(temperature_c);
    char output_buffer[50];
    sprintf(output_buffer, "Temperature: %dC / %dF\r\n", temperature_c, 
            temperature_f);
    UART_write_string(output_buffer);
  }
  else if (strcmp(input, "time") == 0)
  {
    char output_buffer[50];
    sprintf(output_buffer, "Current Time: %02d:%02d:%02d\r\n", RTC->HOUR, 
            RTC->MIN, RTC->SEC);
    UART_write_string(output_buffer);
  }
  else if (strcmp(input, "color") == 0)
  {
    ili9341_fill_screen(ILI9341_BLACK);
    msec_delay(500);
    ili9341_fill_screen(ILI9341_RED);
    msec_delay(500);
    ili9341_fill_screen(ILI9341_GREEN);
    msec_delay(500);
    ili9341_fill_screen(ILI9341_BLUE);
    msec_delay(500);
    ili9341_fill_screen(ILI9341_WHITE);
    msec_delay(500);
  }
  else
  {
    UART_write_string("Unknown command\r\n");
  } 
  UART_write_string("\r\n");
} /* shell_handle_input */
