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
#include "shell.h"
#include "uart.h"
#include "lcd1602.h"
#include "clock.h"


void shell_init(void)
{
  UART_init(115200); //GPIO_CPU_INT_IMASK_DIO15_SET
  uart_interrupt_enable();
} /* shell_init */

void shell_loop(void)
{
  while (1)
  {
    // UART_out_char();
    char buffer[SHELL_MAX_INPUT_LENGTH];
    uint8_t index = 0;
    char input;
    do
    {      
      input = UART_in_char();
      if (input == CARRIAGE_RETURN_CHAR)
      {
        UART_out_char('\n');
        buffer[index] = NULL_CHAR;
      } /* if */
      else if (input == BACKSPACE_CHAR) {
        UART_out_char(input);
        if (index > 0)
        {
          index--;
          lcd_set_ddram_addr(LCD_LINE2_ADDR + index - 1);
          lcd_write_char(' ');
          lcd_set_ddram_addr(LCD_LINE2_ADDR + index - 1);
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
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("                ");
  }
}

void uart_interrupt_enable(void)
{
  UART0->CPU_INT.ICLR = UART_CPU_INT_ICLR_RXINT_CLR;
  UART0->CPU_INT.IMASK = UART_CPU_INT_IMASK_RXINT_SET;
  
  NVIC_EnableIRQ(UART0_INT_IRQn);
} /* uart_interrupt_enable */

void UART0_IRQHandler(void)
{
  uint32_t uart_iidx_status = UART0->CPU_INT.IIDX;

  switch (uart_iidx_status)
  {
    // UART0 receive interrupt
    case (UART_CPU_INT_IIDX_STAT_RXIFG):
      // Manually clear bit to acknowledge interrupt
      UART0->CPU_INT.ICLR = UART_CPU_INT_ICLR_RXINT_CLR;
      
      // char input = UART0->RXDATA;
      // while((UART0->STAT & UART_STAT_TXFF_MASK) == UART_STAT_TXFF_SET);

      // UART0->TXDATA = input;
      break;

    // unexpected interrupt
    default:
      break;
  } /* switch */
} /* UART0_IRQHandler */

void RTC_IRQHandler(void)
{
  uint32_t iidx = RTC->CPU_INT.IIDX;   // Read (clears)
  switch (iidx)
  {
    case RTC_CPU_INT_IIDX_STAT_RTCRDY:
      RTC->CPU_INT.ICLR = RTC_CPU_INT_ICLR_RTCRDY_CLR;
      lcd_write_time(RTC->HOUR, RTC->MIN, RTC->SEC);
      break;
    default:
      break;
  } /* switch */
} /* RTC_IRQHandler */


void lcd_write_time(uint8_t hour, uint8_t min, uint8_t sec)
{
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_char('0' + (hour / 10));
  lcd_write_char('0' + (hour % 10));
  lcd_write_char(':');
  lcd_write_char('0' + (min / 10));
  lcd_write_char('0' + (min % 10));
  lcd_write_char(':');
  lcd_write_char('0' + (sec / 10));
  lcd_write_char('0' + (sec % 10));
} /* lcd_write_time */