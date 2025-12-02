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
  UART_init(BAUD_RATE);
  UART_write_string("\nWelcome back!\n");
  
} /* shell_init */

void shell_loop(void)
{
  while (1)
  {
    char buffer[SHELL_MAX_INPUT_LENGTH];
    uint8_t buffer_index = 0;
    char input;
    do
    {      
      input = UART_in_char();
      if (input == CARRIAGE_RETURN_CHAR)
      {
        UART_out_char(input);
        UART_out_char(NEWLINE_CHAR);
        shell_new_line();
        buffer[buffer_index] = NULL_CHAR;
      } /* if */
      else if (input == BACKSPACE_CHAR) {
        UART_out_char(input);
        if (buffer_index > 0)
        {
          buffer_index--;
          shell_erase_char(buffer[buffer_index]);
        } /* if */
      } /* else if */
      else {
        if (buffer_index < SHELL_MAX_INPUT_LENGTH)
        {
          UART_out_char(input);
          shell_draw_char(input);
          buffer[buffer_index] = input;
          buffer_index++;
        } /* if */
      } /* else */
    } while (input != CARRIAGE_RETURN_CHAR);
    shell_handle_input(buffer);
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
    UART_write_string("  clear - Clear the terminal\r\n");
    shell_draw_string("Available commands:\r\n");
    shell_draw_string("help - Show this help message\r\n");
    shell_draw_string("clock - Measure clock speed\r\n");
    shell_draw_string("temp - Read temperature from thermistor\r\n");
    shell_draw_string("time - Display current RTC time\r\n");
    shell_draw_string("color - Run LCD color test\r\n");
    shell_draw_string("clear - Clear the terminal\r\n");
  } /* if */
  else if (strcmp(input, "clock") == 0)
  {
    uint32_t clock_measurement = measure_clock();
    char output_buffer[50];
    sprintf(output_buffer, "Clock cycles in 100ms: %u\r\n", clock_measurement);
    UART_write_string(output_buffer);
    shell_draw_string(output_buffer);
  } /* else if */
  else if (strcmp(input, "temp") == 0)
  {
    uint16_t adc_temp_result = ADC0_in(TEMP_SENSOR_CHANNEL);
    uint8_t temperature_c = thermistor_calc_temperature(adc_temp_result);
    uint8_t temperature_f = CONVERT_TO_FAHRENHEIT(temperature_c);
    char output_buffer[50];
    sprintf(output_buffer, "Temperature: %dC / %dF\r\n", temperature_c, 
            temperature_f);
    UART_write_string(output_buffer);
    shell_draw_string(output_buffer);
  } /* else if */
  else if (strcmp(input, "time") == 0)
  {
    char output_buffer[50];
    sprintf(output_buffer, "Current Time: %02d:%02d:%02d\r\n", RTC->HOUR, 
            RTC->MIN, RTC->SEC);
    UART_write_string(output_buffer);
    shell_draw_string(output_buffer);
  } /* else if */
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
  } /* else if */
  else if (strcmp(input, "clear") == 0)
  {
    UART_write_string("\033[2J\033[H");
    ili9341_fill_screen(ILI9341_WHITE);
    set_cursor_position(0, SHELL_LINE_HEIGHT);
  } /* else if */
  else
  {
    char* string = "Unknown command\r\n";
    UART_write_string(string);
    shell_draw_string(string);
  } /* else */
  UART_write_string("\r\n");
} /* shell_handle_input */


void shell_draw_char(char c)
{
  ili9341_draw_char_at_cursor(c);
  uint16_t x, y;
  get_cursor_position(&x, &y);
  if (x > ILI9341_TFTWIDTH - GLYPH_WIDTH)
  {
    shell_new_line();
  } /* if */
} /*shell_draw_char */


void shell_erase_char(char c)
{
  uint16_t x, y;
  get_cursor_position(&x, &y);

  if (x == 0 && y >= SHELL_LINE_HEIGHT)
  {
    set_cursor_position(
      (SHELL_CHAR_PER_LINE - 1) * GLYPH_WIDTH, y - SHELL_LINE_HEIGHT
    );
  } /* if */
  else if (x >= GLYPH_WIDTH)
  {
    set_cursor_position(x - GLYPH_WIDTH, y);
  } /* else if */
  ili9341_erase_char(c, ILI9341_WHITE);
} /* shell_erase_char */


void shell_draw_string(char* str)
{
  while (*str != '\0')
  {
    ili9341_draw_char_at_cursor(*str++);
    uint16_t x, y;
    get_cursor_position(&x, &y);
    if (x > ILI9341_TFTWIDTH - GLYPH_WIDTH)
    {
      shell_new_line();
    } /* if */
  } /* while */
} /* shell_draw_string */


void shell_new_line(void)
{
  uint16_t x, y;
  get_cursor_position(&x, &y);
  set_cursor_position(0, y + SHELL_LINE_HEIGHT);
  if (y + SHELL_LINE_HEIGHT > ILI9341_TFTHEIGHT)
  {
    ili9341_fill_screen(ILI9341_WHITE);
    set_cursor_position(0, SHELL_LINE_HEIGHT);
  }
} /* shell_new_line */